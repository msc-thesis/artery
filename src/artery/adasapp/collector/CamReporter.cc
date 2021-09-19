#include "artery/adasapp/collector/CamReporter.h"
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

Define_Module(CamReporter)

static const simsignal_t camRxSignal = cComponent::registerSignal("CamReceived");
#include <iostream>
void CamReporter::initialize(int stage)
{
    // if (stage == artery::InitStages::Prepare) {
    //     camRx = 0;
    // } else if (stage == inet::InitStages::INITSTAGE_APPLICATION_LAYER) {
    //     socket.setOutputGate(gate("socketOut"));
    //     auto centralAddress = inet::L3AddressResolver().resolve(par("centralAddress"));
    //     socket.connect(centralAddress, par("centralPort"));

    //     auto mw = inet::getModuleFromPar<artery::Middleware>(par("middlewareModule"), this);
    //     mw->subscribe(camRxSignal, this);
    // }
    socketIn = gate("socketIn");
    wlanOut = gate("wlanOut");
    ethOut = gate("ethOut");
    socketIn2 = gate("socketIn2");
    socketOut2 = gate("socketOut2");
    socketIn3 = gate("socketIn3");
    port = par("centralPort");
}

int CamReporter::numInitStages() const
{
    return inet::InitStages::INITSTAGE_LAST;
}

void CamReporter::finish()
{
    recordScalar("camRx", camRx);
}

void CamReporter::receiveSignal(cComponent*, simsignal_t sig, cObject* obj, cObject*)
{
    Enter_Method_Silent();
    if (sig == camRxSignal) {
        ++camRx;

        auto* cam = dynamic_cast<artery::CaObject*>(obj);
        //if(cam != nullptr) std::cout << cam->asn1()->header.stationID << std::endl;

        if(cam) {
            // inet::B size((int64_t) cam->asn1().size());
            //std::cout << cam->asn1().size() << std::endl;
            // socket.send(new inet::Packet("DummyCamLog", 
            //             inet::makeShared<inet::ByteCountChunk>(inet::ByteCountChunk(size))));
        }
    }
}

void CamReporter::handleMessage(cMessage *msg) {
    if (msg->getArrivalGate() == socketIn) {
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
            //ethCopy->setControlInfo(geoCtrl);
            ethCopy->addObject(geoCtrl);
        }

        inet::UDPPacket* packet = new inet::UDPPacket("UDP from RSU");
        packet->setDestinationPort(par("centralPort"));
        packet->setControlInfo(ipCtrl);
        packet->encapsulate(check_and_cast<cPacket*>(ethCopy));
        send(packet, ethOut);
    } else if (msg->getArrivalGate() == socketIn2) {
        send(msg, socketOut2);
    } else if (msg->getArrivalGate() == socketIn3) {
        VanetRxControl* ctrl = txToRxControl(check_and_cast<VanetTxControl*>(msg->getObject("")));
        msg->setControlInfo(ctrl);
        send(msg, socketOut2);
    }
}

VanetRxControl* CamReporter::txToRxControl(VanetTxControl* ctrl) {
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
