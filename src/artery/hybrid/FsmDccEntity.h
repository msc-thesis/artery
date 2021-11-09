#ifndef ARTERY_HYBRID_FSMDCCENTITY_H
#define ARTERY_HYBRID_FSMDCCENTITY_H

#include "artery/networking/FsmDccEntity.h"

namespace artery
{

class HybridFsmDccEntity : public FsmDccEntity
{
    public:
        void initialize(int stage) override;
};

} // namespace artery

#endif /* ARTERY_HYBRID_FSMDCCENTITY_H */

