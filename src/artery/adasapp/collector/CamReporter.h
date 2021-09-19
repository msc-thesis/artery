#ifndef CamReporter_H_COLABPR9
#define CamReporter_H_COLABPR9

#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include "artery/inet/VanetRxControl.h"
#include "artery/inet/VanetTxControl.h"

namespace artery {
namespace adasapp {

class CamReporter : public omnetpp::cSimpleModule, public omnetpp::cListener
{
protected:
    virtual int numInitStages() const override;
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

    virtual void handleMessage(omnetpp::cMessage *msg) override;
    VanetRxControl* txToRxControl(VanetTxControl* ctrl);


private:
    void sendReport();

    omnetpp::cGate* socketIn;
    omnetpp::cGate* wlanOut;
    omnetpp::cGate* ethOut;
    omnetpp::cGate* socketIn2;
    omnetpp::cGate* socketOut2;
    omnetpp::cGate* socketIn3;
    int port;

    int camRx;
};

}
}

#endif /* CamReporter_H_COLABPR9 */

