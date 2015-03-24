// P2pFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pFinder::P2pFinder(
            UdpTunnel & tunnel)
            : UdpSession(tunnel)
        {
        }

        P2pFinder::~P2pFinder()
        {
        }

        std::string P2pFinder::proto() const
        {
            return "p2p";
        }

        void P2pFinder::find(
            Resource & resource, 
            size_t count, 
            resp_t const & resp)
        {
            requests_[resource.id()] = std::make_pair(count, resp);
        }

        void P2pFinder::cancel(
            Resource & resource)
        {
            //requests_.erase(resource.id());
        }

        Source * P2pFinder::create(
            Scheduler & scheduler, 
            Url const & url)
        {
            //return new P2pSource(get_io_service(), scheduler, url);
            return NULL;
        }

        void P2pFinder::on_msg(
            Message * msg)
        {
            switch (msg->type) {
            case RSP_Find:
                break;
            case RSP_Register:
                break;
            case RSP_Unregister:
                break;
            default:
                assert(false);
            }
        }

    } // namespace client
} // namespace trip
