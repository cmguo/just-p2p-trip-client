// P2pManager.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/p2p/P2pHttpFinder.h"
#include "trip/client/p2p/P2pPeer.h"
#include "trip/client/udp/UdpManager.h"

namespace trip
{
    namespace client
    {

        P2pManager::P2pManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<P2pManager>(daemon, "P2pManager")
            , umgr_(util::daemon::use_module<UdpManager>(daemon))
            , finder_(new P2pHttpFinder(*this))
        {
        }

        P2pManager::~P2pManager()
        {
            delete finder_;
        }

        bool P2pManager::startup(
            boost::system::error_code & ec)
        {
            ((P2pFinder *)finder_)->open();
            return true;
        }

        bool P2pManager::shutdown(
            boost::system::error_code & ec)
        {
            ((P2pFinder *)finder_)->close();
            return true;
        }

        P2pTunnel & P2pManager::get_tunnel(
            P2pPeer const & peer)
        {
            std::map<Uuid, UdpTunnel *>::iterator iter = tunnels_.find(peer.id);
            if (iter == tunnels_.end()) {
                iter = tunnels_.insert(std::make_pair(peer.id, &umgr_.alloc_tunnel(peer))).first;
            }
            return *iter->second;
        }

        P2pTunnel & P2pManager::get_tunnel(
            Uuid const & pid)
        {
            std::map<Uuid, UdpTunnel *>::iterator iter = tunnels_.find(pid);
            assert(iter != tunnels_.end());
            return *iter->second;
        }

    } // namespace client
} // namespace trip
