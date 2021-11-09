#include "artery/hybrid/networking/AccessInterface.h"
#include "artery/networking/GeoNetPacket.h"
#include "artery/networking/GeoNetRequest.h"
#include <omnetpp/csimplemodule.h>

using vanetza::access::DataRequest;
using vanetza::ChunkPacket;

namespace artery
{

HybridAccessInterface::HybridAccessInterface(omnetpp::cGate* gate, omnetpp::cGate* gateLTE) :
    AccessInterface(gate),
    mGateLTEOut(gateLTE)
{
}

void HybridAccessInterface::requestLTE(const DataRequest& request, std::unique_ptr<ChunkPacket> payload)
{
    // Enter_Method on steroids...
    omnetpp::cMethodCallContextSwitcher ctx(mModuleOut);
    ctx.methodCall("requestLTE");

    GeoNetPacket* gn = new GeoNetPacket("GeoNet packet");
    gn->setPayload(std::move(payload));
    gn->setControlInfo(new GeoNetRequest(request));

    // gn has been created in the context of mModuleOut, thus ownership is fine
    mModuleOut->send(gn, mGateLTEOut);
}

} // namespace artery
