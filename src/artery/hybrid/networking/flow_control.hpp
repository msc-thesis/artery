#ifndef HYBRID_FLOW_CONTROL_HPP
#define HYBRID_FLOW_CONTROL_HPP

#include <vanetza/dcc/flow_control.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <vanetza/net/chunk_packet.hpp>

namespace vanetza
{

// forward declarations
namespace access { class Interface; }
class Runtime;

namespace dcc
{

// forward declarations
class TransmitRateControl;

/**
 * FlowControl is a gatekeeper above access layer.
 *
 * There is a queue for each access category. Packets might be enqueued
 * because of exceeded transmission intervals determined by Scheduler.
 * If a packet's lifetime expires before transmission it will be dropped.
 */
class HybridFlowControl : public FlowControl
{
public:
    HybridFlowControl(Runtime&, TransmitRateControl&, access::Interface&);

    /**
     * Request packet transmission
     * \param request DCC request parameters
     * \param packet Packet data
     */
    void request(const DataRequest&, std::unique_ptr<ChunkPacket>) override;
    void requestLTE(const DataRequest&, std::unique_ptr<ChunkPacket>) override;

private:
    void transmit(const DataRequest&, std::unique_ptr<ChunkPacket>) override;

    bool LTEOnly;
};

} // namespace dcc
} // namespace vanetza

#endif /* HYBRID_FLOW_CONTROL_HPP */

