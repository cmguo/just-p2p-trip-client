// P2pUdpFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pUdpFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSessionListener.h"
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
            framework::network::Endpoint & ep2 = ep.endpoints[0];
            ep2.from_string(urls[0].host_svc());
            attach(&umgr_.get_tunnel(ep));
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
            if (msg.type < REQ_Find) {
                if (tunnel().is_open()) {
                    UdpSession::send_msg(copy);
                } else {
                    pending_msgs_.push_back(copy);
                }
            } else {
                tunnels_[0]->push(copy);
            }
        }

        boost::uint16_t P2pUdpFinder::get_id()
        {
            return l_id();
        }

        void P2pUdpFinder::set_id(
            boost::uint16_t id)
        {
            UdpSession::p_id(id);
            ((UdpSessionListener *)tunnel().main_session())->set_fake_sid(id);
        }

        void P2pUdpFinder::on_msg(
            Message * msg)
        {
            if (!handle_msg(*msg)) {
                pass_msg_to(msg, tunnel().main_session());
                return;
            }
            free_message(msg);
        }

        void P2pUdpFinder::on_tunnel_connecting()
        {
            UdpSession::on_tunnel_connecting();
            open();
            for (size_t i = 0; i < pending_msgs_.size(); ++i)
                UdpSession::send_msg(pending_msgs_[i]);
            pending_msgs_.clear();
        }

        void P2pUdpFinder::on_tunnel_disconnect()
        {
            UdpSession::on_tunnel_disconnect();
            close();
            for (size_t i = 0; i < pending_msgs_.size(); ++i)
                free_message(pending_msgs_[i]);
            pending_msgs_.clear();
        }

    } // namespace client
} // namespace trip
