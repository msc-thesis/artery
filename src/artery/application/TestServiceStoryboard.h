#ifndef TESTSERVICESTORYBOARD_H_
#define TESTSERVICESTORYBOARD_H_

#include "artery/application/TestService.h"

namespace artery
{

class TestServiceStoryboard : public TestService
{
    public:
        void trigger() override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t sig, omnetpp::cObject* sigobj, omnetpp::cObject*);
    
    protected:
        void initialize() override;
        bool triggered;
};

}

#endif /* TESTSERVICE_H_ */

