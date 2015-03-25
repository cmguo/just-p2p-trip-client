// UdpManager.h

#ifndef _TRIP_UDP_UDP_MANAGER_H_
#define _TRIP_UDP_UDP_MANAGER_H_

#include "trip/client/proto/Message.h"

#include <util/daemon/Module.h>

#include <framework/network/Endpoint.h>

#include <boost/function.hpp>

namespace trip
{
    namespace client
    {

        class UdpSocket;
        class UdpTunnel;
        class UdpSession;
        struct UdpPeer;

        class UdpManager
            : public util::daemon::ModuleBase<UdpManager>
        {
        public:
            UdpManager(
                util::daemon::Daemon & daemon);

            ~UdpManager();

        public:
            typedef boost::function<
                UdpSession & (UdpTunnel &, Message const &)> service_t;

            void register_service(
                boost::uint32_t type, 
                service_t service);

        public:
            UdpSocket & socket()
            {
                return *socket_;
            }

            UdpTunnel & get_tunnel(
                Uuid const & pid);

            UdpTunnel & get_tunnel(
                UdpPeer const & peer);

            UdpSession & get_session(
                UdpTunnel & tunnel, 
                Message const & msg);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            framework::network::Endpoint addr_;
            size_t parallel_;
            UdpSocket * socket_;
            std::map<boost::uint32_t, service_t> services_;
            std::map<Uuid, UdpTunnel *> tunnels_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_UDP_UDP_MANAGER_H_
