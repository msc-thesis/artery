#include "artery/hybrid/collector/CarHybridReporter.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/transportlayer/udp/UDPPacket.h"

using namespace omnetpp;

namespace artery
{

namespace hybrid
{

Define_Module(CarHybridReporter)

void CarHybridReporter::initialize(int stage)
{
    registeredToTMC = false;

    if (stage == numInitStages() - 1)
        registerToTMC();

    radioDriverIn = gate("radioDriverGate$i");
    radioDriverOut = gate("radioDriverGate$o");
    wlanIn = gate("wlanGate$i");
    wlanOut = gate("wlanGate$o");
    lteIn = gate("lteGate$i");
    lteOut = gate("lteGate$o");
    centralPort = par("centralPort");
}

int CarHybridReporter::numInitStages() const
{
    return inet::InitStages::INITSTAGE_LAST;
}

void CarHybridReporter::finish() {}

void CarHybridReporter::handleMessage(cMessage *msg) {
    if (checkRegistration(msg))
        return;
    
    if (msg->getArrivalGate() == radioDriverIn) {
        send(msg, wlanOut);

        /*inet::IPv4ControlInfo* ipCtrl = new inet::IPv4ControlInfo();
        ipCtrl->setDestAddr(inet::L3AddressResolver().resolve(par("centralAddress").stringValue()).toIPv4());
        ipCtrl->setProtocol(inet::IP_PROT_UDP);
        ipCtrl->setTimeToLive(10);
        
        auto* ethCopy = msg->dup();
        ethCopy->setName("GeoNet from Car");

        if (ethCopy->getControlInfo() == 0) {
            VanetTxControl* geoCtrl = check_and_cast<VanetTxControl*>(msg->getControlInfo()->dup());
            ethCopy->addObject(geoCtrl);
        }

        inet::UDPPacket* packet = new inet::UDPPacket("UDP from Car");
        packet->setDestinationPort(centralPort);
        packet->setControlInfo(ipCtrl);
        packet->encapsulate(check_and_cast<cPacket*>(ethCopy));
        send(packet, lteOut);*/
    } else if (msg->getArrivalGate() == wlanIn) {
        send(msg, radioDriverOut);
    } else if (msg->getArrivalGate() == lteIn) {
        VanetRxControl* ctrl = txToRxControl(check_and_cast<VanetTxControl*>(msg->removeObject("")));
        cObject* tmp = msg->removeControlInfo();
        delete tmp;
        msg->setControlInfo(ctrl);
        send(msg, radioDriverOut);
    } else {
        delete msg;
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

void CarHybridReporter::registerToTMC() {
    inet::IPv4ControlInfo* ipCtrl = new inet::IPv4ControlInfo();
    ipCtrl->setDestAddr(inet::L3AddressResolver().resolve(par("centralAddress").stringValue()).toIPv4());
    ipCtrl->setProtocol(inet::IP_PROT_UDP);
    ipCtrl->setTimeToLive(10);
    
    inet::UDPPacket* packet = new inet::UDPPacket();
    packet->setDestinationPort(centralPort);
    packet->setControlInfo(ipCtrl);
    packet->encapsulate(new cPacket("TMC registration"));
    send(packet, lteOut);
}

bool CarHybridReporter::checkRegistration(omnetpp::cMessage* msg)
{
    if (strcmp(msg->getName(), "ACK") == 0) {
        registeredToTMC = true;
        delete msg;
        return true;
    } else {
        if (!registeredToTMC)
            registerToTMC();
        return false;
    }
    
}

} // hybrid
} // artery
