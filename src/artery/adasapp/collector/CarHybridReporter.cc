#include "artery/adasapp/collector/CarHybridReporter.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/transportlayer/udp/UDPPacket.h"

using namespace omnetpp;

namespace artery
{

namespace adasapp
{

Define_Module(CarHybridReporter)

void CarHybridReporter::initialize(int stage)
{
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

void CarHybridReporter::finish()
{
    recordScalar("camRx", camRx);
}

void CarHybridReporter::handleMessage(cMessage *msg) {
    if (msg->getArrivalGate() == radioDriverIn) {
        send(msg, wlanOut);

        if (centralPort == 9320)
            return;

        inet::IPv4ControlInfo* ipCtrl = new inet::IPv4ControlInfo();
        ipCtrl->setDestAddr(inet::L3AddressResolver().resolve(par("centralAddress").stringValue()).toIPv4());
        ipCtrl->setProtocol(inet::IP_PROT_UDP);
        ipCtrl->setTimeToLive(1);
        
        auto* ethCopy = msg->dup();
        ethCopy->setName("GeoNet from Car");

        VanetTxControl* geoCtrl = check_and_cast<VanetTxControl*>(msg->getControlInfo()->dup());
        if (ethCopy->getControlInfo() == 0) {
            ethCopy->addObject(geoCtrl);
        }

        inet::UDPPacket* packet = new inet::UDPPacket("UDP from Car");
        packet->setDestinationPort(centralPort);
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

} // adasapp
} // artery
