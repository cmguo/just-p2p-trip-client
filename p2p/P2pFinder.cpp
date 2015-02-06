// P2pFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pFinder.h"
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

        void P2pFinder::find_more(
            Resource & resource)
        {
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
