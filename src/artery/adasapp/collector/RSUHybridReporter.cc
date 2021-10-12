#include "artery/adasapp/collector/RSUHybridReporter.h"
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

namespace artery
{

namespace adasapp
{

Define_Module(RSUHybridReporter)

static const simsignal_t camRxSignal = cComponent::registerSignal("CamReceived");
#include <iostream>
void RSUHybridReporter::initialize(int stage)
{
    radioIn = gate("radioIn");
    radioOut = gate("radioOut");
    wlanIn = gate("wlanIn");
    wlanOut = gate("wlanOut");
    ethOut = gate("ethOut");
    port = par("centralPort");
}

int RSUHybridReporter::numInitStages() const
{
    return inet::InitStages::INITSTAGE_LAST;
}

void RSUHybridReporter::finish()
{
    recordScalar("camRx", camRx);
}

void RSUHybridReporter::handleMessage(cMessage *msg) {
    if (msg->getArrivalGate() == radioIn) {
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
        send(packet, ethOut);
    } else if (msg->getArrivalGate() == wlanIn) {
        send(msg, radioOut);
    }
}

} // Adasapp
} // artery
