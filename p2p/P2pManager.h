// P2pManager.h

#ifndef _TRIP_P2P_P2P_MANAGER_H_
#define _TRIP_P2P_P2P_MANAGER_H_

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        struct P2pPeer;

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
            P2pTunnel & get_tunnel(
                Uuid const & pid);

            P2pTunnel & get_tunnel(
                P2pPeer const & peer);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            UdpManager & umgr_;
            Finder * finder_;
            std::map<Uuid, P2pTunnel *> tunnels_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_P2P_P2P_MANAGER_H_
