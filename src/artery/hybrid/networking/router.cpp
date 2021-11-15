#include "artery/hybrid/networking/router.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/geonet/cbf_counter.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/dcc_field_generator.hpp>
#include <vanetza/geonet/next_hop.hpp>
#include <vanetza/geonet/pdu_conversion.hpp>
#include <boost/variant.hpp>

namespace vanetza
{
namespace geonet
{

namespace
{

struct ControlInfo
{
    ControlInfo(const DataRequest request) :
        communication_profile(request.communication_profile),
        its_aid(request.its_aid) {}

    const CommunicationProfile communication_profile;
    const ItsAid its_aid;
};

template<typename PDU>
class PendingPacketBufferData : public packet_buffer::Data
{
public:
    PendingPacketBufferData(PendingPacket<PDU>&& packet) : m_packet(std::move(packet)) {}

    std::size_t length() const override
    {
        return m_packet.length();
    }

    Clock::duration reduce_lifetime(Clock::duration d) override
    {
        return m_packet.reduce_lifetime(d);
    }

    void flush() override
    {
        m_packet.process();
    }

protected:
    PendingPacket<PDU> m_packet;
};

dcc::RequestInterface* get_default_request_interface()
{
    static dcc::NullRequestInterface null;
    return &null;
}

DccFieldGenerator* get_default_dcc_field_generator()
{
    static NullDccFieldGenerator null;
    return &null;
}

std::unique_ptr<CbfCounter> create_cbf_counter(Runtime& rt, const MIB& mib)
{
    std::unique_ptr<CbfCounter> counter;
    if (mib.vanetzaFadingCbfCounter) {
        counter.reset(new CbfCounterFading(rt, units::clock_cast(mib.vanetzaFadingCbfCounterLifetime)));
    } else {
        counter.reset(new CbfCounterContending());
    }
    assert(counter);
    return counter;
}

} // namespace

using units::clock_cast;
using PendingPacketGbc = PendingPacket<GbcPdu>;


HybridRouter::HybridRouter(Runtime& rt, const MIB& mib) : Router(rt, mib)
{
    LTEOnly = false;
}

DataConfirm HybridRouter::request(const ShbDataRequest& request, DownPacketPtr payload)
{
    DataConfirm result;
    result ^= validate_data_request(request, m_mib);
    result ^= validate_payload(payload, m_mib);

    if (!result.accepted())
        return result;
    
    using PendingPacket = PendingPacket<ShbPdu>;

    // step 4: set up packet repetition (NOTE 4 on page 57 requires re-execution of source operations)
    if (request.repetition) {
        // plaintext payload needs to get passed
        m_repeater.add(request, *payload);
    }

    // step 1: create PDU
    auto pdu = create_shb_pdu(request);
    pdu->common().payload = payload->size();

    ControlInfo ctrl(request);
    auto transmit = [this, ctrl](PendingPacket::Packet&& packet) {
        std::unique_ptr<ShbPdu> pdu;
        std::unique_ptr<DownPacket> payload;
        std::tie(pdu, payload) = std::move(packet);

        // update SO PV before actual transmission
        pdu->extended().source_position = m_local_position_vector;

        // step 2: encapsulate packet by security
        if (m_mib.itsGnSecurity) {
            payload = encap_packet(ctrl.its_aid, *pdu, std::move(payload));
        }

        // step 5: execute media-dependent procedures
        execute_media_procedures(ctrl.communication_profile);

        // step 6: pass packet down to link layer with broadcast destination
        pass_down(cBroadcastMacAddress, std::move(pdu), std::move(payload));

        // step 7: reset beacon timer
        reset_beacon_timer();
    };

    PendingPacket packet(std::make_tuple(std::move(pdu), std::move(payload)), transmit);

    // step 3: store & carry forwarding
    if (request.traffic_class.store_carry_forward() && !m_location_table.has_neighbours()) {
        PacketBuffer::data_ptr data { new PendingPacketBufferData<ShbPdu>(std::move(packet)) };
        m_bc_forward_buffer.push(std::move(data), m_runtime.now());
    } else {
        // tranmsit immediately
        packet.process();
    }

    return result;
}

DataConfirm HybridRouter::request(const GbcDataRequest& oldRequest, DownPacketPtr payload)
{
    using vanetza::units::si::meter;
    DataConfirm result;
    result ^= validate_payload(payload, m_mib);

    if (!result.accepted())
        return result;

    result = validate_data_request(oldRequest, m_mib);

    GbcDataRequest request = oldRequest;

    if (!result.accepted()) {
        result = DataConfirm::ResultCode::Accepted;
        LTEOnly = true;

        Rectangle shape;
        shape.a = 10000.0 * meter;
        shape.b = 10000.0 * meter;

        request.destination.shape = shape;
    }

    // step 6: set up packet repetition
    // packet repetition is done first because of "NOTE 2" on page 60:
    // "For every retransmission, the source operations need to be re-executed".
    // Hence, all routing decisions and security encapsulation have to be performed again.
    // Assumption: "omit execution of further steps" does not cancel the repetition procedure.
    if (request.repetition) {
        m_repeater.add(request, *payload);
    }

    using PendingPacket = PendingPacket<GbcPdu>;
    using Packet = PendingPacket::Packet;

    // step 1: create PDU and set header fields
    auto pdu = create_gbc_pdu(request);
    pdu->common().payload = payload->size();

    ControlInfo ctrl(request);
    auto transmit = [this, ctrl](Packet&& packet, const MacAddress& mac) {
        std::unique_ptr<GbcPdu> pdu;
        std::unique_ptr<DownPacket> payload;
        std::tie(pdu, payload) = std::move(packet);

        // update SO PV before actual transmission
        pdu->extended().source_position = m_local_position_vector;

        // step 5: apply security
        if (m_mib.itsGnSecurity) {
            assert(pdu->basic().next_header == NextHeaderBasic::Secured);
            payload = encap_packet(ctrl.its_aid, *pdu, std::move(payload));
        }

        // step 6: repetition is already set-up before

        // step 7: execute media-dependent procedures
        execute_media_procedures(ctrl.communication_profile);

        // step 8: pass PDU to link layer
        pass_down(mac, std::move(pdu), std::move(payload));
    };

    auto forwarding = [this, transmit](Packet&& packet) {
        // step 3: forwarding algorithm selection procedure
        NextHop nh = forwarding_algorithm_selection(PendingPacketForwarding(std::move(packet), transmit), nullptr);

        // step 4: omit execution of further steps when packet if buffered or discarded
        std::move(nh).process();
    };

    PendingPacket packet(std::make_tuple(std::move(pdu), std::move(payload)), forwarding);

    // step 2: check if neighbours are present
    const bool scf = request.traffic_class.store_carry_forward();
    if (scf && !m_location_table.has_neighbours()) {
        PacketBuffer::data_ptr data { new PendingPacketBufferData<GbcPdu>(std::move(packet)) };
        m_bc_forward_buffer.push(std::move(data), m_runtime.now());
    } else {
        packet.process();
    }

    return result;
}

void HybridRouter::pass_down(const dcc::DataRequest& request, PduPtr pdu, DownPacketPtr payload)
{
    assert(pdu);
    assert(payload);
    if (pdu->secured()) {
        if (pdu->basic().next_header != NextHeaderBasic::Secured) {
            throw std::runtime_error("PDU with secured message but Secured not set in basic header");
        }
        if (payload->size(OsiLayer::Transport, max_osi_layer()) > 0) {
            throw std::runtime_error("PDU with secured message and illegal upper layer payload");
        }
    } else {
        if (pdu->basic().next_header == NextHeaderBasic::Secured) {
            throw std::runtime_error("PDU without secured message but Secured set in basic header");
        }
    }

    (*payload)[OsiLayer::Network] = ByteBufferConvertible(std::move(pdu));
    assert(m_request_interface);

    if (LTEOnly)
    {
        m_request_interface->requestLTE(request, std::move(payload));
        LTEOnly = false;
    }
    else
        m_request_interface->request(request, std::move(payload));
}

void HybridRouter::pass_down(const MacAddress& addr, PduPtr pdu, DownPacketPtr payload)
{
    assert(pdu);

    dcc::DataRequest request;
    request.destination = addr;
    request.source = m_local_position_vector.gn_addr.mid();
    request.dcc_profile = map_tc_onto_profile(pdu->common().traffic_class);
    request.ether_type = geonet::ether_type;
    request.lifetime = std::chrono::seconds(pdu->basic().lifetime.decode() / units::si::seconds);

    pass_down(request, std::move(pdu), std::move(payload));
}


} // namespace geonet
} // namespace vanetza
