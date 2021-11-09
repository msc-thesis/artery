#ifndef ARTERY_HYBRID_ACCESSINTERFACE_H
#define ARTERY_HYBRID_ACCESSINTERFACE_H

#include <omnetpp/cgate.h>
#include "artery/networking/AccessInterface.h"

// forward declarations
namespace omnetpp
{
class cGate;
class cSimpleModule;
} // namespace omnetpp

namespace artery
{

class HybridAccessInterface : public AccessInterface
{
public:
    HybridAccessInterface(omnetpp::cGate*, omnetpp::cGate*);

private:
    omnetpp::cGate* mGateLTEOut;
};

} // namespace artery

#endif /* ARTERY_HYBRID_ACCESSINTERFACE_H */

