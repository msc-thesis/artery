/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_RSUDENSERVICE_H_
#define ARTERY_RSUDENSERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include <vanetza/asn1/denm.hpp>
#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/asn1/its/ReferencePosition.h>

namespace artery
{

class Timer;

class RsuDenService : public ItsG5BaseService
{
    public:
        RsuDenService();
        ~RsuDenService();
        void initialize() override;
        void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;

        void sendDenm(vanetza::asn1::Denm&&, vanetza::btp::DataRequestB&);

    private:
        vanetza::btp::DataRequestB createRequest(const vanetza::btp::DataIndication&, const vanetza::asn1::Denm&, bool = false);
        void fillRequest(vanetza::btp::DataRequestB&);

        std::vector<ReferencePosition_t> positions;
};

} // namespace artery

#endif
