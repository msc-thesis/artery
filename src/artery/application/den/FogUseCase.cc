#include "artery/application/den/FogUseCase.h"
#include "artery/application/DenService.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/application/VehicleDataProvider.h"
#include <boost/units/systems/si/prefixes.hpp>
#include <math.h>

namespace artery
{
namespace den
{

static const auto microdegree = vanetza::units::degree * boost::units::si::micro;

template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}

double deg2rad(double deg)
{
    return deg / 180 * M_PI;
}

// Distance between two points on the surface of earth according to the Haversine formula
double distance(double lat1, double lon1, double lat2, double lon2)
{
    // Radius of the Earth in meters, because we want to return the distance in meters
    double R = 6371000;
    double dLat = deg2rad(lat2 - lat1);
    double dLon = deg2rad(lon2 - lon1);
    double a = 
        sin(dLat / 2) * sin(dLat / 2) +
        cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
        sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = R * c;

    return d;
}

bool contains(std::vector<ReferencePosition_t> container, ReferencePosition_t element)
{
    for (auto itr = container.begin(); itr != container.end(); itr++)
    {
        if (itr->latitude == element.latitude && itr->longitude == element.longitude)
            return true;
    }
    return false;
}

StationType_t castStationType(VehicleDataProvider::StationType inType)
{
    switch (inType)
    {
        case VehicleDataProvider::StationType::Unknown: return StationType_unknown;
        case VehicleDataProvider::StationType::Pedestrian: return StationType_pedestrian;
        case VehicleDataProvider::StationType::Cyclist: return StationType_cyclist;
        case VehicleDataProvider::StationType::Moped: return StationType_moped;
        case VehicleDataProvider::StationType::Motorcycle: return StationType_motorcycle;
        case VehicleDataProvider::StationType::Passenger_Car: return StationType_passengerCar;
        case VehicleDataProvider::StationType::Bus: return StationType_bus;
        case VehicleDataProvider::StationType::Light_Truck: return StationType_lightTruck;
        case VehicleDataProvider::StationType::Heavy_Truck: return StationType_heavyTruck;
        case VehicleDataProvider::StationType::Trailer: return StationType_trailer;
        case VehicleDataProvider::StationType::Special_Vehicle: return StationType_specialVehicles;
        case VehicleDataProvider::StationType::Tram: return StationType_tram;
        case VehicleDataProvider::StationType::RSU: return StationType_roadSideUnit;
        default: return StationType_unknown;
    }
}

Define_Module(artery::den::FogUseCase);

FogUseCase::~FogUseCase()
{
    positions.clear();
}

void FogUseCase::initialize(int stage)
{
    UseCase::initialize(stage);

    vehicleLatitude.setName("Vehicle latitude");
    vehicleLongitude.setName("Vehicle longitude");
    eventLatitude.setName("Event latitude");
    eventLongitude.setName("Event longitude");
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

    if (asn1->denm.situation->eventType.causeCode != CauseCodeType_adverseWeatherCondition_Visibility)
        return;

    double vLat = round(mVdp->latitude(), microdegree) * Latitude_oneMicrodegreeNorth / 10000000.0;
    double vLon = round(mVdp->longitude(), microdegree) * Longitude_oneMicrodegreeEast / 10000000.0;
    double eLat = asn1->denm.management.eventPosition.latitude / 10000000.0;
    double eLon = asn1->denm.management.eventPosition.longitude / 10000000.0;

    if (contains(positions, asn1->denm.management.eventPosition))
        return;

    positions.push_back(asn1->denm.management.eventPosition);
    
    vehicleLatitude.record(vLat);
    vehicleLongitude.record(vLon);
    eventLatitude.record(eLat);
    eventLongitude.record(eLon);

    double dist = distance(vLat, vLon, eLat, eLon);

    if (dist > 500)
        printf("Dist: %f, I'm well informed\n", dist);

    printf("FOG indicated !!!\n");

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
    // The distance within which the event is considered relevant to the receiving ITS-S 
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan5km;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_allTrafficDirections;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 600;
    msg->denm.management.stationType = castStationType(mVdp->getStationType());

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
    shape.r = 1784.0 * meter;
    destination.shape = shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;
    request.gn.maximum_lifetime = geonet::Lifetime { geonet::Lifetime::Base::Ten_Seconds, 60 };

    return request;
}

} // namespace den
} // namespace artery
