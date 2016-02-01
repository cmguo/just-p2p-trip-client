// UdpManager.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpPacket.h"
#include "trip/client/udp/UdpTunnelListener.h"
#include "trip/client/udp/UdpSessionListener.h"
#include "trip/client/udp/UdpSessionListener2.h"
#include "trip/client/timer/TimerManager.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/network/Interface.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.UdpManager", framework::logger::Debug);

        UdpManager::UdpManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<UdpManager>(daemon, "trip.client.UdpManager")
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
            , addr_("0.0.0.0:6666")
            , parallel_(10)
            , socket_(new UdpSocket(io_svc()))
        {
            module_config()
                << CONFIG_PARAM_NAME_NOACC("addr", addr_)
                << CONFIG_PARAM_NAME_NOACC("parallel", parallel_)
                << CONFIG_PARAM_NAME_NOACC("uid", local_endpoint_.id);
            UdpTunnel * tunnel = new UdpTunnel(*socket_);
            new UdpTunnelListener(*this, *tunnel);
            LOG_INFO("[sizeof] Message:" << sizeof(Message) << ", UdpPacket:" << sizeof(UdpPacket));
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
            if (local_endpoint_.id == Uuid()) {
                local_endpoint_.id.generate();
                config().set_force(name(), "uid", local_endpoint_.id.to_string(), true);
            }
            std::vector<framework::network::Interface> interfaces;
            enum_interface(interfaces);
            Endpoint ep;
            ep.port(addr_.port());
            for (size_t i = 0; i < interfaces.size(); ++i) {
                framework::network::Interface ifc(interfaces[i]);
                if ((ifc.flags & (ifc.up | ifc.loopback)) == ifc.up) {
                    ep.ip(ifc.addr);
                    local_endpoint_.endpoints.push_back(ep);
                }
            }
            return socket_->start(addr_, parallel_, ec);
        }

        bool UdpManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_10_ms.un(
                boost::bind(&UdpManager::on_event, this, _1, _2));
            return socket_->stop(ec);
        }

        void UdpManager::register_service(
            boost::uint32_t type, 
            service_t service)
        {
            services_[type] = service;
        }

        UdpSession * UdpManager::get_session(
            UdpTunnel & tunnel, 
            Message & msg)
        {
            std::map<boost::uint32_t, service_t>::const_iterator iter = services_.find(msg.id());
            if (iter == services_.end())
                return NULL;
            return iter->second(tunnel, msg);
        }

        UdpTunnel * UdpManager::get_tunnel(
            Uuid const & uid)
        {
            std::map<Uuid, UdpTunnel *>::iterator iter = tunnels_.find(uid);
            if (iter == tunnels_.end()) {
                return NULL;
            }
            return iter->second;
        }

        UdpTunnel & UdpManager::get_tunnel(
            UdpEndpoint const & endpoint)
        {
            std::map<Uuid, UdpTunnel *>::iterator iter = tunnels_.find(endpoint.id);
            if (iter == tunnels_.end()) {
                UdpTunnel * tunnel = new UdpTunnel(*socket_);
                UdpSessionListener * listener = new UdpSessionListener(*this, *tunnel);
                iter = tunnels_.insert(std::make_pair(endpoint.id, tunnel)).first;
                listener->set_remote(endpoint);
            }
            return *iter->second;
        }

        UdpTunnel & UdpManager::get_tunnel(
            UdpEndpoint const & endpoint, 
            UdpSession * session)
        {
            UdpTunnel * tunnel = new UdpTunnel(*socket_);
            new UdpSessionListener2(*this, *tunnel, endpoint, session);
            return *tunnel;
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
