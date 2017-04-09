// P2pUdpFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pUdpFinder.h"
#include "trip/client/p2p/P2pSource.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSessionListener.h"
#include "trip/client/proto/MessageTracker.h"
#include "trip/client/proto/Message.hpp"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.P2pUdpFinder", framework::logger::Debug);

        P2pUdpFinder::P2pUdpFinder(
           P2pManager & manager)
            : P2pFinder(manager)
            , index_(-1)
        {
            ::srand(::time(NULL));
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
            endpoints_.resize(urls.size());
            for (size_t i = 0; i < urls.size(); ++i) {
                UdpEndpoint & ep = endpoints_[i];
                ep.endpoints.resize(1);
                ep.id.from_bytes(framework::string::md5(urls[i].host_svc()).to_bytes());
                framework::network::Endpoint & ep2 = ep.endpoints[0];
                ep2.from_string(urls[i].host_svc());
            }
            index_ = ::rand() % urls.size();
            attach(&umgr_.get_tunnel(endpoints_[index_]));
            for (size_t i = 0; i < urls.size(); ++i) {
                tunnels_.push_back(&umgr_.get_tunnel(endpoints_[i], this));
            }
        }

        void P2pUdpFinder::send_msg(
            Message const & msg)
        {
            if (msg.type < REQ_Find) {
                Message * copy = alloc_message();
                *copy = msg;
                if (tunnel().is_open()) {
                    UdpSession::send_msg(copy);
                } else {
                    pending_msgs_.push_back(copy);
                }
            } else {
                for (size_t i = 0; i < tunnels_.size(); ++i) {
                    Message * copy = alloc_message();
                    *copy = msg;
                    tunnels_[i]->push(copy);
                }
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
            Time now;
            if (now > tunnel().stat().recv_bytes().time + Duration::seconds(10)) {
                if (endpoints_.size() > 1) {
                    size_t index = ::rand() % endpoints_.size();
                    while (index == index_)
                        index = ::rand() % endpoints_.size();
                    index_ = index;
                    detach();
                    attach(&umgr_.get_tunnel(endpoints_[index]));
                    return;
                }
            }
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
