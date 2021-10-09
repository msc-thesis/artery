#ifndef ARTERY_DEN_FOGUSECASE
#define ARTERY_DEN_FOGUSECASE

#include "artery/application/den/UseCase.h"

namespace artery
{
namespace den
{

class FogUseCase : public UseCase
{
public:
    // UseCase interface
    void check() override;
    void indicate(const artery::DenmObject&) override;
    void handleStoryboardTrigger(const StoryboardSignal&) override;

    vanetza::asn1::Denm createMessage(RequestResponseIndication_t);
    vanetza::btp::DataRequestB createRequest();

private:
    bool mPendingRequest = false;
    void transmitMessage(RequestResponseIndication_t);
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_FOGUSECASE */
