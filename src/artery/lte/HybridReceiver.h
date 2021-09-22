//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_HybridReceiver_H_
#define _LTE_HybridReceiver_H_

#include <string.h>
#include <omnetpp.h>

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include <list>

namespace artery
{

class HybridReceiver : public cSimpleModule
{
    inet::UDPSocket socket;

    ~HybridReceiver();
    std::list<inet::UDPPacket*> packetsList;

    unsigned int totalRcvdBytes_;
    simtime_t warmUpPer_;

    simsignal_t voIPReceivedThroughput_;

    omnetpp::cGate* geoNetOut;

    virtual void finish();

  protected:

    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
    void initialize(int stage);
    void handleMessage(cMessage *msg);
};

}

#endif

