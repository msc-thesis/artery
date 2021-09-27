#ifndef TESTSERVICE_H_
#define TESTSERVICE_H_

#include "artery/application/ItsG5Service.h"
#include "artery/traci/VehicleController.h"

namespace artery
{

class TestService : public artery::ItsG5Service
{
    public:
        void trigger() override;
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
    
    protected:
        void initialize() override;
    
    private:
        const traci::VehicleController* mVehicleController = nullptr;
};

}

#endif /* TESTSERVICE_H_ */
