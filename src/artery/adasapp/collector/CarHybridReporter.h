#ifndef CarHybridReporter_H
#define CarHybridReporter_H

#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include "artery/inet/VanetRxControl.h"
#include "artery/inet/VanetTxControl.h"

namespace artery
{

namespace adasapp
{

class CarHybridReporter : public omnetpp::cSimpleModule, public omnetpp::cListener
{
protected:
    virtual int numInitStages() const override;
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override {};
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    VanetRxControl* txToRxControl(VanetTxControl* ctrl);

private:
    omnetpp::cGate* radioDriverIn;
    omnetpp::cGate* radioDriverOut;
    omnetpp::cGate* wlanIn;
    omnetpp::cGate* wlanOut;
    omnetpp::cGate* lteIn;
    omnetpp::cGate* lteOut;
    int port;
    int camRx;
};

} // adasapp
} // artery

#endif /* CarHybridReporter_H */

