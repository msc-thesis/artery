#ifndef ARTERY_DEN_FOGUSECASE
#define ARTERY_DEN_FOGUSECASE

#include "artery/application/den/SuspendableUseCase.h"
#include <vanetza/btp/data_request.hpp>

namespace artery
{
namespace den
{

class FogUseCase : public SuspendableUseCase
{
public:
    void check() override;
    void indicate(const artery::DenmObject&) override;
    void handleStoryboardTrigger(const StoryboardSignal&) override;
    
    vanetza::asn1::Denm createMessage();
    vanetza::btp::DataRequestB createRequest();

protected:
    void initialize(int stage) override;

private:
    bool mPendingSignal = false;
    void transmitMessage();
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_FOGUSECASE */
