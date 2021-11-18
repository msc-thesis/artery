#ifndef _LTE_HybridSender_H_
#define _LTE_HybridSender_H_

#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace artery
{

class HybridSender : public cSimpleModule
{
    inet::UDPSocket socket;
    int localPort_;
    int destPort_;
    std::set<inet::L3Address> addresses;
    inet::L3AddressResolver* resolver;

    void sendPacket(cPacket* packet);
    void ackRegistration(inet::L3Address);

  protected:
    virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
    void initialize(int stage);
    void handleMessage(cMessage *msg);
  
  public:
    ~HybridSender();

};

} // artery

#endif

