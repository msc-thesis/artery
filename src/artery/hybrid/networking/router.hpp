#ifndef HYBRID_ROUTER_HPP
#define HYBRID_ROUTER_HPP

#include <vanetza/geonet/router.hpp>

namespace vanetza
{

namespace geonet
{

class HybridRouter : public Router
{
    public:
        HybridRouter(Runtime&, const MIB&);
        DataConfirm request(const ShbDataRequest&, DownPacketPtr) override;
        DataConfirm request(const GbcDataRequest&, DownPacketPtr) override;
    
    private:
        void pass_down(const dcc::DataRequest&, PduPtr, DownPacketPtr) override;
        void pass_down(const MacAddress&, PduPtr, DownPacketPtr) override;
};

} // namespace geonet
} // namespace vanetza

#endif /* HYBRID_ROUTER_HPP */
