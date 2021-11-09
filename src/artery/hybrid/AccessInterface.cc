#include "artery/hybrid/AccessInterface.h"

using vanetza::access::DataRequest;
using vanetza::ChunkPacket;

namespace artery
{

HybridAccessInterface::HybridAccessInterface(omnetpp::cGate* gate, omnetpp::cGate* gateLTE) :
    AccessInterface(gate),
    mGateLTEOut(gateLTE)
{
}

} // namespace artery
