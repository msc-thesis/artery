#ifndef _LTE_HybridReceiver_H_
#define _LTE_HybridReceiver_H_

#include <string.h>
#include <omnetpp.h>

#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include <list>

namespace artery
{

class HybridReceiver : public cSimpleModule
{
    inet::UDPSocket socket;
    omnetpp::cGate* geoNetOut;

  protected:
    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
    void initialize(int stage);
    void handleMessage(cMessage *msg);
};

}

#endif

