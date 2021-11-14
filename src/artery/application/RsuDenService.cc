/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/DenmObject.h"
#include "artery/application/RsuDenService.h"
#include <vanetza/asn1/denm.hpp>
#include <vanetza/btp/ports.hpp>
#include <vanetza/asn1/its/ReferencePosition.h>

using namespace omnetpp;

namespace artery
{

bool contains(std::vector<ReferencePosition_t> container, ReferencePosition_t element)
{
    for (auto itr = container.begin(); itr != container.end(); itr++)
    {
        if (itr->latitude == element.latitude && itr->longitude == element.longitude)
            return true;
    }
    return false;
}

Define_Module(RsuDenService)

static const simsignal_t denmReceivedSignal = cComponent::registerSignal("DenmReceived");
static const simsignal_t denmSentSignal = cComponent::registerSignal("DenmSent");

RsuDenService::RsuDenService()
{
}

RsuDenService::~RsuDenService()
{
    positions.clear();
}

void RsuDenService::initialize()
{
    ItsG5BaseService::initialize();
}

void RsuDenService::indicate(const vanetza::btp::DataIndication& indication, std::unique_ptr<vanetza::UpPacket> packet)
{
    Asn1PacketVisitor<vanetza::asn1::Denm> visitor;
    const vanetza::asn1::Denm* denm = boost::apply_visitor(visitor, *packet);
    vanetza::asn1::Denm den = vanetza::asn1::Denm(*denm);

    if (denm)
    {
        DenmObject obj = visitor.shared_wrapper;
        const vanetza::asn1::Denm& asn1 = obj.asn1();

        if (contains(positions, asn1->denm.management.eventPosition))
            return;

        positions.push_back(asn1->denm.management.eventPosition);

        vanetza::btp::DataRequestB request = createRequest(indication, asn1);
        sendDenm(std::move(den), request);
    }
}

void RsuDenService::sendDenm(vanetza::asn1::Denm&& message, vanetza::btp::DataRequestB& request)
{
    fillRequest(request);
    DenmObject obj { std::move(message) };
    emit(denmSentSignal, &obj);

    using namespace vanetza;
    using DenmConvertible = vanetza::convertible::byte_buffer_impl<vanetza::asn1::Denm>;
    std::unique_ptr<geonet::DownPacket> payload { new geonet::DownPacket };
    std::unique_ptr<vanetza::convertible::byte_buffer> denm { new DenmConvertible { obj.shared_ptr() } };
    payload->layer(OsiLayer::Application) = vanetza::ByteBufferConvertible { std::move(denm) };
    this->request(request, std::move(payload));
}

vanetza::btp::DataRequestB RsuDenService::createRequest(const vanetza::btp::DataIndication& indication, const vanetza::asn1::Denm& asn1)
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    
    request.gn.traffic_class.tc_id(indication.traffic_class.raw());

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 1.0 * seconds;
    repetition.maximum = 300.0 * seconds;
    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Rectangle shape;
    shape.a = 1000.0 * meter;
    shape.b = 2500.0 * meter;
    destination.shape = shape;

    ReferencePosition_t position = asn1->denm.management.eventPosition;

    vanetza::geonet::GeodeticPosition p(position.latitude * vanetza::units::degree, position.longitude * vanetza::units::degree);
    destination.position = p;
    request.gn.destination = destination;
    request.gn.maximum_lifetime = geonet::Lifetime { geonet::Lifetime::Base::Ten_Seconds, 60 };

    return request;
}

void RsuDenService::fillRequest(vanetza::btp::DataRequestB& request)
{
    using namespace vanetza;

    request.destination_port = btp::ports::DENM;
    request.gn.its_aid = aid::DEN;
    request.gn.transport_type = geonet::TransportType::GBC;
    request.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
}

} // namespace artery
