// UdpManager.h

#ifndef _TRIP_UDP_UDP_MANAGER_H_
#define _TRIP_UDP_UDP_MANAGER_H_

#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/proto/Message.h"

#include <util/daemon/Module.h>
#include <util/event/Event.h>

#include <framework/network/Endpoint.h>

#include <boost/function.hpp>

namespace trip
{
    namespace client
    {

        class UdpSocket;
        class UdpTunnel;
        class UdpSession;

        class TimerManager;

        class UdpManager
            : public util::daemon::ModuleBase<UdpManager>
        {
        public:
            UdpManager(
                util::daemon::Daemon & daemon);

            ~UdpManager();

        public:
            typedef boost::function<
                UdpSession * (UdpTunnel &, Message &)> service_t;

            void register_service(
                boost::uint32_t type, 
                service_t service);

        public:
            UdpSocket & socket()
            {
                return *socket_;
            }

            UdpEndpoint & local_endpoint()
            {
                return local_endpoint_;
            }

            std::map<Uuid, UdpTunnel *> const & tunnels() const
            {
                return tunnels_;
            }

            std::vector<UdpTunnel *> const & stateless_tunnels() const
            {
                return stateless_tunnels_;
            }

        public:
            UdpTunnel * get_tunnel(
                Uuid const & uid);

            UdpTunnel & get_tunnel(
                UdpEndpoint const & endpoint);

            // tunnel with only one session, stateless
            UdpTunnel & get_tunnel(
                UdpEndpoint const & endpoint, 
                UdpSession * session);

            void free_tunnel(
                Uuid const & uid);

            UdpSession * get_session(
                UdpTunnel & tunnel, 
                Message & msg);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

        private:
            TimerManager & tmgr_;
            framework::network::Endpoint addr_;
            UdpEndpoint local_endpoint_;
            size_t parallel_;
            UdpSocket * socket_;
            std::map<boost::uint32_t, service_t> services_;
            std::map<Uuid, UdpTunnel *> tunnels_;
            std::vector<UdpTunnel *> stateless_tunnels_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_MANAGER_H_
