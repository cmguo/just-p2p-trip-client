// UdpManager.cpp

#include "trip/client/Common.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpSocket.h"
#include "trip/client/udp/UdpTunnel.h"
#include "trip/client/udp/UdpListener.h"

namespace trip
{
    namespace client
    {

        UdpManager::UdpManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<UdpManager>(daemon)
            , addr_(":6666")
            , parallel_(10)
            , socket_(new UdpSocket(io_svc()))
        {
            config().register_module("CacheManager")
                << CONFIG_PARAM_NAME_NOACC("addr", addr_)
                << CONFIG_PARAM_NAME_NOACC("parallel", parallel_);
            UdpTunnel * main_tunnel = new UdpTunnel(*socket_);
            new UdpListener(*main_tunnel);
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

    } // namespace client
} // namespace trip
