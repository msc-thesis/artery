#include "artery/hybrid/networking/Router.h"
#include "artery/application/Middleware.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include "artery/networking/IDccEntity.h"
#include "artery/networking/Runtime.h"
#include "artery/networking/SecurityEntity.h"
#include <inet/common/ModuleAccess.h>
#include <boost/units/cmath.hpp>
#include <boost/units/io.hpp>

namespace vanetza {
namespace geonet {

static inline std::ostream& operator<<(std::ostream& os, const vanetza::geonet::LongPositionVector& epv)
{
    using namespace boost::units;
    os << "\n"
        << "latitude: \t" << abs(epv.position().latitude) << (epv.latitude.value() < 0 ? " S" : " N") << "\n"
        << "longitude: \t" << abs(epv.position().longitude) << (epv.longitude.value() < 0 ? " W" : " E") << "\n"
        << "heading: \t" << vanetza::units::GeoAngle { epv.heading };
    return os;
}

} // namespace geonet
} // namespace vanetza

namespace artery
{

Define_Module(HybridRouter)

static const omnetpp::simsignal_t scPositionFixSignal = omnetpp::cComponent::registerSignal("PositionFix");

void HybridRouter::initialize(int stage)
{
    Router::initialize(stage);

    if (stage == InitStages::Self) {
        // basic router setup
        auto runtime = inet::getModuleFromPar<Runtime>(par("runtimeModule"), this);
        mRouter.reset(new vanetza::geonet::Router(*runtime, mMIB));
        vanetza::MacAddress init_mac = vanetza::create_mac_address(getId());
        mRouter->set_address(generateAddress(init_mac));

        // register security entity if available
        if (mSecurityEntity) {
            mRouter->set_security_entity(mSecurityEntity);
        }

        // bind router to DCC entity
        auto dccEntity = inet::findModuleFromPar<IDccEntity>(par("dccModule"), this);
        mRouter->set_access_interface(notNullPtr(dccEntity->getRequestInterface()));
        mRouter->set_dcc_field_generator(dccEntity->getGeonetFieldGenerator()); // nullptr is okay

        // pass BTP-B messages to transport layer dispatcher in network interface
        using vanetza::geonet::UpperProtocol;
        mNetworkInterface = std::make_shared<NetworkInterface>(*this, *dccEntity, mMiddleware->getTransportDispatcher());
        mRouter->set_transport_handler(UpperProtocol::BTP_B, &mNetworkInterface->getTransportHandler());

        // finally, register new network interface at middleware
        mMiddleware->registerNetworkInterface(mNetworkInterface);

        omnetpp::createWatch("EPV", mRouter->get_local_position_vector());
    }
}

} // namespace artery
