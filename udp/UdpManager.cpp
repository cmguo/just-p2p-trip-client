// UdpManager.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpListener.h"
#include "trip/client/udp/UdpMainSession.h"
#include "trip/client/udp/UdpPeer.h"

namespace trip
{
    namespace client
    {

        UdpManager::UdpManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<UdpManager>(daemon, "UdpManager")
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
            return socket_->start(addr_, parallel_, ec);
        }

        bool UdpManager::shutdown(
            boost::system::error_code & ec)
        {
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

    } // namespace client
} // namespace trip
