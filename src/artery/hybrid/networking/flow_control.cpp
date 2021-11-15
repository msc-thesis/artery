#include "artery/hybrid/networking/flow_control.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <vanetza/access/data_request.hpp>
#include <vanetza/access/interface.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/dcc/mapping.hpp>

namespace vanetza
{
namespace dcc
{

HybridFlowControl::HybridFlowControl(Runtime& runtime, TransmitRateControl& trc, access::Interface& ifc) :
    FlowControl(runtime, trc, ifc)
{
    LTEOnly = false;
}

void HybridFlowControl::request(const DataRequest& request, std::unique_ptr<ChunkPacket> packet)
{
    drop_expired();

    const TransmissionLite transmission { request.dcc_profile, packet->size() };
    if (transmit_immediately(transmission)) {
        m_trc.notify(transmission);
        transmit(request, std::move(packet));
    } else {
        enqueue(request, std::move(packet));
    }
}

void HybridFlowControl::requestLTE(const DataRequest& request, std::unique_ptr<ChunkPacket> packet)
{
    LTEOnly = true;
    HybridFlowControl::request(request, std::move(packet));
}

void HybridFlowControl::transmit(const DataRequest& request, std::unique_ptr<ChunkPacket> packet)
{
    access::DataRequest access_request;
    access_request.source_addr = request.source;
    access_request.destination_addr = request.destination;
    access_request.ether_type = request.ether_type;
    access_request.access_category = map_profile_onto_ac(request.dcc_profile);

    m_packet_transmit_hook(access_request.access_category, packet.get());

    if (LTEOnly)
    {
        m_access.requestLTE(access_request, std::move(packet));
        LTEOnly = false;
    }
    else
        m_access.request(access_request, std::move(packet));
}

} // namespace dcc
} // namespace vanetza
