#include "artery/application/TestService.h"
#include "artery/traci/VehicleController.h"
#include "artery/application/TestMsg_m.h"

using namespace omnetpp;
using namespace vanetza;

namespace artery
{

Define_Module(TestService)

void TestService::initialize()
{
    ItsG5Service::initialize();
    mVehicleController = &getFacilities().get_const<traci::VehicleController>();
}

void TestService::trigger()
{
    Enter_Method("TestService trigger");

    auto msg = new TestMsg();
    msg->setName("Hello World!");
    msg->setIndex(42);

    btp::DataRequestB req;
    req.destination_port = host_cast<TestService::port_type>(getPortNumber());
    req.gn.transport_type = geonet::TransportType::SHB;
    req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
    req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
    request(req, msg);
}

void TestService::indicate(const vanetza::btp::DataIndication& ind, omnetpp::cPacket* packet)
{
    Enter_Method("TestService indicate");

    auto msg = check_and_cast<const TestMsg*>(packet);

    EV << msg->getName() << endl;
    EV << msg->getIndex() << endl;

    delete msg;
}

}