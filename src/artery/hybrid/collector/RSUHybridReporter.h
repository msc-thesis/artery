#ifndef RSUHybridReporter_H
#define RSUHybridReporter_H

#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{

namespace hybrid
{

class RSUHybridReporter : public omnetpp::cSimpleModule, public omnetpp::cListener
{
protected:
    virtual int numInitStages() const override;
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override {};
    virtual void handleMessage(omnetpp::cMessage *msg) override;

private:
    omnetpp::cGate* radioDriverIn;
    omnetpp::cGate* radioDriverOut;
    omnetpp::cGate* wlanIn;
    omnetpp::cGate* wlanOut;
    omnetpp::cGate* ethIn;
    omnetpp::cGate* ethOut;
    int centralPort;
};

} // hybrid
} // artery

#endif /* RSUHybridReporter_H */

