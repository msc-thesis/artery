//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "artery/lte/HybridReceiver.h"
#include "artery/networking/GeoNetPacket.h"

namespace artery
{

Define_Module(HybridReceiver);

HybridReceiver::~HybridReceiver()
{
    while (!packetsList.empty()) {
        delete packetsList.front();
        packetsList.pop_front();
    }
}

void HybridReceiver::initialize(int stage)
{
    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    int port = par("localPort");
    geoNetOut = gate("geoNetOut");

    EV << "HybridReceiver::initialize - binding to port: local:" << port << endl;
    if (port != -1)
    {
        socket.setOutputGate(gate("udpOut"));
        socket.bind(port);
    }

    totalRcvdBytes_ = 0;
    warmUpPer_ = getSimulation()->getWarmupPeriod();

    voIPReceivedThroughput_ = registerSignal("voIPReceivedThroughput");
}

void HybridReceiver::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        return;

    inet::UDPPacket* pPacket = check_and_cast<inet::UDPPacket*>(msg);
    GeoNetPacket* decapsulatedPacket = check_and_cast<GeoNetPacket*>(pPacket->decapsulate());

    // emit throughput sample
    totalRcvdBytes_ += (int)pPacket->getByteLength();
    double interval = SIMTIME_DBL(simTime() - warmUpPer_);
    if (interval > 0.0)
    {
        double tputSample = (double)totalRcvdBytes_ / interval;
        emit(voIPReceivedThroughput_, tputSample );
    }
    
    send(decapsulatedPacket, geoNetOut);

    pPacket->setArrivalTime(simTime());
    packetsList.push_back(pPacket);
}

void HybridReceiver::finish()
{
}

}