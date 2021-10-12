#include "artery/adasapp/collector/CarHybridReporter.h"
#include "artery/application/Middleware.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/application/cpacket_byte_buffer_convertible.h"
#include "artery/traci/VehicleController.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <omnetpp/checkandcast.h>
#include "artery/networking/GeoNetPacket.h"
#include <inet/networklayer/contract/ipv4/IPv4ControlInfo.h>
#include "inet/transportlayer/udp/UDPPacket.h"

using namespace omnetpp;

namespace artery {
namespace adasapp {

Define_Module(CarHybridReporter)

static const simsignal_t camRxSignal = cComponent::registerSignal("CamReceived");
#include <iostream>
void CarHybridReporter::initialize(int stage)
{
    radioDriverIn = gate("radioDriverGate$i");
    radioDriverOut = gate("radioDriverGate$o");
    wlanIn = gate("wlanGate$i");
    wlanOut = gate("wlanGate$o");
    lteIn = gate("lteGate$i");
    lteOut = gate("lteGate$o");
    port = par("centralPort");
}

int CarHybridReporter::numInitStages() const
{
    return inet::InitStages::INITSTAGE_LAST;
}

void CarHybridReporter::finish()
{
    recordScalar("camRx", camRx);
}

void CarHybridReporter::handleMessage(cMessage *msg) {
    if (msg->getArrivalGate() == radioDriverIn) {
        send(msg, wlanOut);

        if (port == 9320)
            return;

        inet::IPv4ControlInfo* ipCtrl = new inet::IPv4ControlInfo();
        ipCtrl->setDestAddr(inet::L3AddressResolver().resolve(par("centralAddress").stringValue()).toIPv4());
        ipCtrl->setProtocol(inet::IP_PROT_UDP);
        ipCtrl->setTimeToLive(1);
        
        auto* ethCopy = msg->dup();
        ethCopy->setName("From RSU");

        VanetTxControl* geoCtrl = check_and_cast<VanetTxControl*>(msg->getControlInfo()->dup());
        if (ethCopy->getControlInfo() == 0) {
            ethCopy->addObject(geoCtrl);
        }

        inet::UDPPacket* packet = new inet::UDPPacket("UDP from RSU");
        packet->setDestinationPort(par("centralPort"));
        packet->setControlInfo(ipCtrl);
        packet->encapsulate(check_and_cast<cPacket*>(ethCopy));
        send(packet, lteOut);
    } else if (msg->getArrivalGate() == wlanIn) {
        send(msg, radioDriverOut);
    } else if (msg->getArrivalGate() == lteIn) {
        VanetRxControl* ctrl = txToRxControl(check_and_cast<VanetTxControl*>(msg->getObject("")));
        msg->removeControlInfo();
        msg->setControlInfo(ctrl);
        send(msg, radioDriverOut);
    }
}

VanetRxControl* CarHybridReporter::txToRxControl(VanetTxControl* ctrl) {
    VanetRxControl* tmp = new VanetRxControl();
    tmp->setSrc(ctrl->getSrc());
    tmp->setDest(ctrl->getDest());
    tmp->setEtherType(ctrl->getEtherType());
    tmp->setInterfaceId(ctrl->getInterfaceId());
    tmp->setSwitchPort(ctrl->getSwitchPort());
    tmp->setUserPriority(ctrl->getUserPriority());
    tmp->setSsap(ctrl->getSsap());
    tmp->setDsap(ctrl->getDsap());
    tmp->setPauseUnits(ctrl->getPauseUnits());
    return tmp;
}

}
}
