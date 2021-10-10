#include "artery/application/den/FogUseCase.h"
#include "artery/application/DenService.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/application/VehicleDataProvider.h"

namespace artery
{
namespace den
{

Define_Module(artery::den::FogUseCase)

void FogUseCase::initialize(int stage)
{
    UseCase::initialize(stage);
}

void FogUseCase::check()
{
    Enter_Method("FogUseCase: check");

    if (!isDetectionBlocked() && mPendingSignal) {
        blockDetection();
        mPendingSignal = false;
        printf("FOG !!!\n");
        transmitMessage();
    }
}

void FogUseCase::indicate(const artery::DenmObject& denm)
{
    Enter_Method("FogUseCase: indicate");
    
    const vanetza::asn1::Denm& asn1 = denm.asn1();

    if (asn1->denm.situation->eventType.causeCode == CauseCodeType_adverseWeatherCondition_Visibility)
    {

    }
}

void FogUseCase::transmitMessage()
{
    Enter_Method("FogUseCase: transmitMessage");

    auto denm = createMessage();
    auto request = createRequest();
    mService->sendDenm(std::move(denm), request);
}

void FogUseCase::handleStoryboardTrigger(const StoryboardSignal& signal)
{
    Enter_Method("FogUseCase: handleStoryboardTrigger");

    if (signal.getCause() == "fog") {
        mPendingSignal = true;
    }
}

vanetza::asn1::Denm FogUseCase::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan1000m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_allTrafficDirections;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 600;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_adverseWeatherCondition_Visibility;
    msg->denm.situation->eventType.subCauseCode = 0;

    return msg;
}

vanetza::btp::DataRequestB FogUseCase::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 1.0 * seconds;
    repetition.maximum = 300.0 * seconds;
    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle shape;
    shape.r = 1000.0 * meter;
    destination.shape = shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;
    request.gn.maximum_lifetime = geonet::Lifetime { geonet::Lifetime::Base::Ten_Seconds, 60 };

    return request;
}

} // namespace den
} // namespace artery
