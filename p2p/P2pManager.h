// P2pManager.h

#ifndef _TRIP_P2P_P2P_MANAGER_H_
#define _TRIP_P2P_P2P_MANAGER_H_

#include "trip/client/proto/Message.h"

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        struct UdpEndpoint;

        class ResourceManager;
        class UdpManager;
        class Finder;
        class UdpTunnel;

        typedef UdpTunnel P2pTunnel;

        class P2pManager
            : public util::daemon::ModuleBase<P2pManager>
        {
        public:
            P2pManager(
                util::daemon::Daemon & daemon);

            ~P2pManager();

        public:
            Finder * finder()
            {
                return finder_;
            }

        public:
            UdpEndpoint & local_endpoint();

            P2pTunnel * get_tunnel(
                Uuid const & pid);

            P2pTunnel & get_tunnel(
                UdpEndpoint const & endpoint);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            ResourceManager & rmgr_;
            UdpManager & umgr_;
            Finder * finder_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_P2P_MANAGER_H_
