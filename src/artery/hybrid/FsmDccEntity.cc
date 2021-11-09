#include "artery/hybrid/FsmDccEntity.h"
#include "artery/hybrid/AccessInterface.h"
#include "artery/utility/InitStages.h"

namespace artery
{

Define_Module(HybridFsmDccEntity)

void HybridFsmDccEntity::initialize(int stage)
{
    DccEntityBase::initialize(stage);

    if (stage == InitStages::Prepare)
    {
        mAccessInterface.reset(new HybridAccessInterface(gate("radioDriverData"), gate("LTEOut")));
    }
}

} // namespace artery
