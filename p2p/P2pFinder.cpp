// P2pFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpPeer.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pFinder::P2pFinder(
            UdpManager & manager)
            : UdpSession(manager.get_tunnel(Uuid()))
            , umgr_(manager)
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
            size_t count)
        {
        }

        Source * P2pFinder::create(
            Resource & resource, 
            Url const & url)
        {
            UdpPeer peer;
            Uuid rid;
            return new P2pSource(resource, umgr_.get_tunnel(peer), url);
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
