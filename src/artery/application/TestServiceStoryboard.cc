#include "artery/application/TestServiceStoryboard.h"
#include "artery/application/StoryboardSignal.h"

using namespace omnetpp;

namespace artery
{

static const simsignal_t storyboardSignal = cComponent::registerSignal("StoryboardSignal");

Define_Module(TestServiceStoryboard);

void TestServiceStoryboard::initialize()
{
    TestService::initialize();
    subscribe(storyboardSignal);
    triggered = false;
}

void TestServiceStoryboard::trigger()
{
    Enter_Method("TestServiceStoryboard trigger");
    if (triggered) {
        TestService::trigger();
    }
}

void TestServiceStoryboard::receiveSignal(cComponent*, simsignal_t sig, cObject* sigobj, cObject*)
{
    if (sig == storyboardSignal) {
        auto storysig = dynamic_cast<StoryboardSignal*>(sigobj);
        if (storysig && storysig->getCause() == "slow down") {
            triggered = true;
        }
    }
}

}