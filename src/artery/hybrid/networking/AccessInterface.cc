#include "artery/hybrid/networking/AccessInterface.h"
#include "artery/networking/GeoNetPacket.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/utility/PointerCheck.h"
#include "artery/inet/VanetTxControl.h"
#include <omnetpp/csimplemodule.h>

using vanetza::access::DataRequest;
using vanetza::ChunkPacket;

namespace artery
{

namespace
{

vanetza::MacAddress convert(const inet::MACAddress& mac)
{
	vanetza::MacAddress result;
	mac.getAddressBytes(result.octets.data());
	return result;
}

inet::MACAddress convert(const vanetza::MacAddress& mac)
{
	inet::MACAddress result;
	result.setAddressBytes(const_cast<uint8_t*>(mac.octets.data()));
	return result;
}

}

HybridAccessInterface::HybridAccessInterface(omnetpp::cGate* gate, omnetpp::cGate* gateLTE) :
    AccessInterface(gate),
    mGateLTEOut(notNullPtr(gateLTE))
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

    handleDataRequest(gn);

    // gn has been created in the context of mModuleOut, thus ownership is fine
    mModuleOut->send(gn, mGateLTEOut);
}

void HybridAccessInterface::handleDataRequest(omnetpp::cMessage* packet)
{
	auto request = check_and_cast<GeoNetRequest*>(packet->removeControlInfo());
	auto ctrl = new VanetTxControl();
	ctrl->setDest(convert(request->destination_addr));
	ctrl->setSourceAddress(convert(request->source_addr));
	ctrl->setEtherType(request->ether_type.host());
    switch (request->access_category) {
		case vanetza::access::AccessCategory::VO:
			ctrl->setUserPriority(7);
			break;
		case vanetza::access::AccessCategory::VI:
			ctrl->setUserPriority(5);
			break;
		case vanetza::access::AccessCategory::BE:
			ctrl->setUserPriority(3);
			break;
		case vanetza::access::AccessCategory::BK:
			ctrl->setUserPriority(1);
			break;
		default:
			throw cRuntimeError("mapping to user priority (UP) unknown");
	}
    packet->setControlInfo(ctrl);
    delete request;
}

} // namespace artery
