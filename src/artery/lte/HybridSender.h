//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_HybridSender_H_
#define _LTE_HybridSender_H_

#include <string.h>
#include <omnetpp.h>

#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "artery/networking/GeoNetPacket.h"

namespace artery
{

class HybridSender : public cSimpleModule
{
    inet::UDPSocket socket;
    int localPort_;
    int destPort_;
    std::set<inet::L3Address> addresses;

    void sendPacket(cPacket* packet);

  protected:
    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
    void initialize(int stage);
    void handleMessage(cMessage *msg);

};

} // artery

#endif

