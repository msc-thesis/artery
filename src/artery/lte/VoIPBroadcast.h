//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_VoIPBroadcast_H_
#define _LTE_VoIPBroadcast_H_

#include <string.h>
#include <omnetpp.h>

#include "artery/lte/VoipPacket_m.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "artery/networking/GeoNetPacket.h"

namespace artery
{

class VoIPBroadcast : public cSimpleModule
{
    inet::UDPSocket socket;

    //source
    cMessage* selfSource_;
    //sender
    int nframesTmp_;
    int size_;
    simtime_t sampling_time;

    unsigned int totalSentBytes_;
    simtime_t warmUpPer_;

    simsignal_t voIPGeneratedThroughtput_;
    // ----------------------------

    cMessage *selfSender_;

    cMessage *initTraffic_;

    int localPort_;
    int destPort_;
    inet::L3Address destAddress_;

    void initTraffic();
    void sendVoIPPacket(GeoNetPacket* p = nullptr);

  public:
    ~VoIPBroadcast();
    VoIPBroadcast();

  protected:

    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
    void initialize(int stage);
    void handleMessage(cMessage *msg);

};

}

#endif

