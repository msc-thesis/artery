#include "artery/hybrid/networking/flow_control.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <vanetza/access/data_request.hpp>
#include <vanetza/access/interface.hpp>
#include <vanetza/common/runtime.hpp>

namespace vanetza
{
namespace dcc
{

HybridFlowControl::HybridFlowControl(Runtime& runtime, TransmitRateControl& trc, access::Interface& ifc) :
    FlowControl(runtime, trc, ifc)
{
}

void HybridFlowControl::request(const DataRequest& request, std::unique_ptr<ChunkPacket> packet)
{
    printf("new request\n");
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

}

} // namespace dcc
} // namespace vanetza
