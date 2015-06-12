// UdpManager.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpListener.h"
#include "trip/client/udp/UdpMainSession.h"
#include "trip/client/udp/UdpPeer.h"
#include "trip/client/timer/TimerManager.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        UdpManager::UdpManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<UdpManager>(daemon, "UdpManager")
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
            , addr_(":6666")
            , parallel_(10)
            , socket_(new UdpSocket(io_svc()))
        {
            config().register_module("UdpManager")
                << CONFIG_PARAM_NAME_NOACC("addr", addr_)
                << CONFIG_PARAM_NAME_NOACC("parallel", parallel_);
            new UdpListener(*this);
        }

        UdpManager::~UdpManager()
        {
            delete socket_;
        }

        bool UdpManager::startup(
            boost::system::error_code & ec)
        {
            tmgr_.t_10_ms.on(
                boost::bind(&UdpManager::on_event, this, _1, _2));
            return socket_->start(addr_, parallel_, ec);
        }

        bool UdpManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_10_ms.un(
                boost::bind(&UdpManager::on_event, this, _1, _2));
            return socket_->stop(ec);
        }

        UdpTunnel & UdpManager::get_tunnel(
            UdpPeer const & peer)
        {
            UdpTunnel & tunnel = get_tunnel(peer.id);
            new UdpMainSession(tunnel, peer);
            return tunnel;
        }

        UdpTunnel & UdpManager::get_tunnel(
            Uuid const & pid)
        {
            std::map<Uuid, UdpTunnel *>::iterator iter = tunnels_.find(pid);
            if (iter == tunnels_.end()) {
                iter = tunnels_.insert(std::make_pair(pid, new UdpTunnel(*socket_))).first;
            }
            UdpTunnel * tunnel = iter->second;
            return *tunnel;
        }

        UdpSession & UdpManager::get_session(
            UdpTunnel & tunnel, 
            Message const & msg)
        {
            std::map<boost::uint32_t, service_t>::const_iterator iter = services_.find(msg.id());
            if (iter == services_.end())
                return *new UdpSession(tunnel);
            return iter->second(tunnel, msg);
        }

        void UdpManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == tmgr_) {
                socket_->handle_timer(tmgr_.t_10_ms.now);
            }
        }

    } // namespace client
} // namespace trip
