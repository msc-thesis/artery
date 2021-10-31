#ifndef ARTERY_DEN_FOGUSECASE
#define ARTERY_DEN_FOGUSECASE

#include "artery/application/den/SuspendableUseCase.h"
#include <vanetza/btp/data_request.hpp>
#include "vanetza/asn1/its/ReferencePosition.h"

namespace artery
{
namespace den
{

class FogUseCase : public SuspendableUseCase
{
public:
    ~FogUseCase();

    void check() override;
    void indicate(const artery::DenmObject&) override;
    void handleStoryboardTrigger(const StoryboardSignal&) override;
    
    vanetza::asn1::Denm createMessage();
    vanetza::btp::DataRequestB createRequest();

protected:
    void initialize(int stage) override;

private:
    bool mPendingSignal = false;
    std::vector<ReferencePosition_t> positions;
    void transmitMessage();
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_FOGUSECASE */
