// P2pUdpFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pUdpFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpPeer.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pUdpFinder::P2pUdpFinder(
           P2pManager & manager)
            : P2pFinder(manager)
            , UdpSession(manager.get_tunnel(Uuid()))
        {
        }

        P2pUdpFinder::~P2pUdpFinder()
        {
        }

        void P2pUdpFinder::send_msg(
            Message const & msg)
        {
            Message copy(msg);
            send_msg(copy);
        }

        void P2pUdpFinder::on_msg(
            Message * msg)
        {
            handle_msg(*msg);
        }

    } // namespace client
} // namespace trip
