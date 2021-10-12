#include "artery/lte/HybridSender.h"
#include "artery/networking/GeoNetPacket.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

namespace artery
{

Define_Module(HybridSender);

void HybridSender::initialize(int stage)
{
    EV << "VoIP Sender initialize: stage " << stage << endl;

    cSimpleModule::initialize(stage);

    // avoid multiple initializations
    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    localPort_ = par("localPort");
    destPort_ = par("destPort");


    EV << "HybridSender::initialize - binding to port: local:" << localPort_ << endl;

    socket.setOutputGate(gate("udpOut"));
    socket.bind(localPort_);
}

void HybridSender::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage()) {
        const char* destAddress = par("destAddress").stringValue();

        inet::UDPDataIndication* udpControlInfo = check_and_cast<inet::UDPDataIndication*>(msg->removeControlInfo());
        inet::L3Address srcAddr = udpControlInfo->getSrcAddr();

        cModule* destModule = getModuleByPath(destAddress);

        if (destModule != nullptr) {
            destAddress_ = inet::L3AddressResolver().resolve(destAddress);
            msg->removeControlInfo();
            socket.sendTo(check_and_cast<cPacket*>(msg), destAddress_, destPort_);
        } else {
            delete msg;
        }
    }
}

} // artery
