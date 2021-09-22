//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include <cmath>
#include "artery/lte/HybridSender.h"
#include "artery/networking/GeoNetPacket.h"
#include "inet/transportlayer/udp/UDPPacket.h"

#define round(x) floor((x) + 0.5)

namespace artery
{

Define_Module(HybridSender);

HybridSender::HybridSender()
{
}

HybridSender::~HybridSender()
{
}

void HybridSender::initialize(int stage)
{
    EV << "VoIP Sender initialize: stage " << stage << endl;

    cSimpleModule::initialize(stage);

    // avoid multiple initializations
    if (stage!=inet::INITSTAGE_APPLICATION_LAYER)
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
        cModule* destModule = getModuleByPath(destAddress);
        

        if (destModule != nullptr) {
            destAddress_ = inet::L3AddressResolver().resolve(destAddress);
            sendUDPPacket(dynamic_cast<GeoNetPacket*>(msg));
        }
    }
}

void HybridSender::sendUDPPacket(GeoNetPacket* p)
{
    inet::UDPPacket* packet = new inet::UDPPacket("GeoNet inside");
    int udpPacketLength = packet->getByteLength();
    int geoNetPacketLength = p->getByteLength();
    packet->encapsulate(p);
    packet->setTimestamp(simTime());
    packet->setByteLength(udpPacketLength + geoNetPacketLength);
    socket.sendTo(packet, destAddress_, destPort_);
}

}