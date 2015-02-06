// UdpManager.h

#ifndef _TRIP_P2P_UDP_MANAGER_H_
#define _TRIP_P2P_UDP_MANAGER_H_

#include <util/daemon/Module.h>

#include <framework/network/Endpoint.h>

namespace trip
{
    namespace client
    {

        class UdpSocket;

        class UdpManager
            : public util::daemon::ModuleBase<UdpManager>
        {
        public:
            UdpManager(
                util::daemon::Daemon & daemon);

            ~UdpManager();

        public:

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            framework::network::Endpoint addr_;
            size_t parallel_;
            UdpSocket * socket_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_UDP_MANAGER_H_
