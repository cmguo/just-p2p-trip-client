// P2pUdpFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pUdpFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        P2pUdpFinder::P2pUdpFinder(
           P2pManager & manager)
            : P2pFinder(manager)
        {
        }

        P2pUdpFinder::~P2pUdpFinder()
        {
        }

        void P2pUdpFinder::init()
        {
            std::vector<Url> urls;
            Bootstrap::instance(pmgr_.io_svc()).get("tracker", urls);
            if (urls.empty())
                urls.push_back(Url("http://tracker.trip.com/"));
            UdpEndpoint ep;
            ep.endpoints.resize(1);
            framework::network::Endpoint & ep2 = ep.endpoints[0].endp;
            for (size_t i = 0; i < urls.size(); ++i) {
                Url const & url(urls[i]);
                ep2.from_string(url.host_svc());
                tunnels_.push_back(&umgr_.get_tunnel(ep, this));
            }
        }

        void P2pUdpFinder::send_msg(
            Message const & msg)
        {
            Message * copy = alloc_message();
            *copy = msg;
            UdpSession::send_msg(copy);
        }

        boost::uint16_t P2pUdpFinder::get_id()
        {
            return id();
        }

        void P2pUdpFinder::on_msg(
            Message * msg)
        {
            handle_msg(*msg);
        }

    } // namespace client
} // namespace trip
