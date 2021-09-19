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
    selfSource_ = NULL;
    selfSender_ = NULL;
}

HybridSender::~HybridSender()
{
    cancelAndDelete(selfSource_);
    cancelAndDelete(selfSender_);
}

void HybridSender::initialize(int stage)
{
    EV << "VoIP Sender initialize: stage " << stage << endl;

    cSimpleModule::initialize(stage);

    // avoid multiple initializations
    if (stage!=inet::INITSTAGE_APPLICATION_LAYER)
        return;

    selfSource_ = new cMessage("selfSource");
    nframesTmp_ = 0;
    size_ = par("PacketSize");
    sampling_time = par("sampling_time");
    selfSender_ = new cMessage("selfSender");
    localPort_ = par("localPort");
    destPort_ = par("destPort");

    totalSentBytes_ = 0;
    warmUpPer_ = getSimulation()->getWarmupPeriod();
    voIPGeneratedThroughtput_ = registerSignal("voIPGeneratedThroughput");

    EV << "HybridSender::initialize - binding to port: local:" << localPort_ << endl;

    socket.setOutputGate(gate("udpOut"));
    socket.bind(localPort_);

    initTraffic_ = new cMessage("initTraffic");
    initTraffic();
}

void HybridSender::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
    //     if (!strcmp(msg->getName(), "selfSender"))
    //         sendVoIPPacket();
    //     else if (!strcmp(msg->getName(), "selfSource"))
    //         selectPeriodTime();
    //     else
            // initTraffic();
    } else {
        const char* destAddress = par("destAddress").stringValue();
        cModule* destModule = getModuleByPath(destAddress);
        

        if (destModule != nullptr) {
            destAddress_ = inet::L3AddressResolver().resolve(destAddress);
            sendVoIPPacket(dynamic_cast<GeoNetPacket*>(msg));
        }
    }
}

void HybridSender::initTraffic()
{
    const char* destAddress = par("destAddress").stringValue();
    cModule* destModule = getModuleByPath(destAddress);
    if (destModule == NULL)
    {
        // this might happen when users are created dynamically
        EV << simTime() << "HybridSender::initTraffic - destination " << destAddress << " not found" << endl;

        simtime_t offset = 0.01; // TODO check value
        scheduleAt(simTime()+offset, initTraffic_);
        EV << simTime() << "HybridSender::initTraffic - the node will retry to initialize traffic in " << offset << " seconds " << endl;
    }
    else
    {
        delete initTraffic_;

        destAddress_ = inet::L3AddressResolver().resolve(destAddress);
        // socket.setOutputGate(gate("udpOut"));
        // socket.bind(localPort_);

        EV << simTime() << "HybridSender::initialize - binding to port: local:" << localPort_ << " , dest: " << destAddress_.str() << ":" << destPort_ << endl;

        // calculating traffic starting time
        simtime_t startTime = par("startTime");

        // TODO maybe un-necesessary
        // this conversion is made in order to obtain ms-aligned start time, even in case of random generated ones
        simtime_t offset = (round(SIMTIME_DBL(startTime)*1000)/1000);

        scheduleAt(simTime()+startTime, selfSource_);
        EV << "\t starting traffic in " << startTime << " seconds " << endl;
    }
}

void HybridSender::sendVoIPPacket(GeoNetPacket* p)
{
    GeoNetPacket* gn = new GeoNetPacket("GeoNet packet");
    // if (p)
    //     gn = p;

    inet::UDPPacket* packet = new inet::UDPPacket("GeoNet inside");
    int udpPacketLength = packet->getByteLength();
    int geoNetPacketLength = p->getByteLength();
    packet->encapsulate(p);
    packet->setTimestamp(simTime());
    packet->setByteLength(udpPacketLength + geoNetPacketLength);
    EV << destAddress_ << endl;
    EV << destPort_ << endl;
    socket.sendTo(packet, destAddress_, destPort_);
    --nframesTmp_;

    // emit throughput sample
    totalSentBytes_ += size_;
    double interval = SIMTIME_DBL(simTime() - warmUpPer_);
    if (interval > 0.0)
    {
        double tputSample = (double)totalSentBytes_ / interval;
        emit(voIPGeneratedThroughtput_, tputSample );
    }

    if (nframesTmp_ > 0)
        scheduleAt(simTime() + sampling_time, selfSender_);
}

}