#ifndef ARTERY_HYBRID_ROUTER_H
#define ARTERY_HYBRID_ROUTER_H

#include "artery/networking/Router.h"

namespace artery
{

class HybridRouter : public Router 
{
    public:
        void initialize(int stage);
};

} // namespace artery

#endif /* ARTERY_HYBRID_ROUTER_H */
