#include "artery/hybrid/collector/RSUHybridReporter.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include "artery/inet/VanetTxControl.h"

using namespace omnetpp;

namespace artery
{

namespace hybrid
{

Define_Module(RSUHybridReporter)

void RSUHybridReporter::initialize(int stage)
{
    radioDriverIn = gate("radioDriverGate$i");
    radioDriverOut = gate("radioDriverGate$o");
    wlanIn = gate("wlanGate$i");
    wlanOut = gate("wlanGate$o");
    ethOut = gate("ethOut");
    centralPort = par("centralPort");
}

int RSUHybridReporter::numInitStages() const
{
    return inet::InitStages::INITSTAGE_LAST;
}

void RSUHybridReporter::finish() {}

void RSUHybridReporter::handleMessage(cMessage *msg) {
    if (msg->getArrivalGate() == radioDriverIn) {
        send(msg, wlanOut);

        inet::IPv4ControlInfo* ipCtrl = new inet::IPv4ControlInfo();
        ipCtrl->setDestAddr(inet::L3AddressResolver().resolve(par("centralAddress").stringValue()).toIPv4());
        ipCtrl->setProtocol(inet::IP_PROT_UDP);
        ipCtrl->setTimeToLive(10);
        
        auto* ethCopy = msg->dup();
        ethCopy->setName("GeoNet from RSU");

        if (ethCopy->getControlInfo() == 0) {
            VanetTxControl* geoCtrl = check_and_cast<VanetTxControl*>(msg->getControlInfo()->dup());
            ethCopy->addObject(geoCtrl);
        }

        inet::UDPPacket* packet = new inet::UDPPacket("UDP from RSU");
        packet->setDestinationPort(centralPort);
        packet->setControlInfo(ipCtrl);
        packet->encapsulate(check_and_cast<cPacket*>(ethCopy));
        send(packet, ethOut);
    } else if (msg->getArrivalGate() == wlanIn) {
        send(msg, radioDriverOut);
    } else {
        delete msg;
    }
}

} // hybrid
} // artery
