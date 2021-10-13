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
        inet::UDPDataIndication* udpControlInfo = check_and_cast<inet::UDPDataIndication*>(msg->removeControlInfo());
        inet::L3Address srcAddr = udpControlInfo->getSrcAddr();

        if (srcAddr.getPrefix(16) != inet::L3Address("192.168.0.0"))
            addresses.insert(srcAddr);

        sendPacket(check_and_cast<cPacket*>(msg));
    }
}

void HybridSender::sendPacket(cPacket* packet)
{
    std::set<inet::L3Address>::iterator itr;

    for (itr = addresses.begin(); itr != addresses.end(); itr++)
    {
        auto* tmp = packet->dup();
        socket.sendTo(tmp, *itr, destPort_);
    }
    delete packet;
}

} // artery
