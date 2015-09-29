// P2pManager.cpp

#include "trip/client/Common.h"
#include "trip/client/proto/MessageSession.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/p2p/P2pHttpFinder.h"
#include "trip/client/p2p/P2pSink.h"
#include "trip/client/udp/UdpManager.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/main/ResourceManager.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        P2pManager::P2pManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<P2pManager>(daemon, "P2pManager")
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
            , umgr_(util::daemon::use_module<UdpManager>(daemon))
            , finder_(new P2pHttpFinder(*this))
        {
            umgr_.register_service(REQ_Bind, 
                boost::bind(&P2pSink::create_session, boost::ref(rmgr_), _1, _2));
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

        UdpEndpoint & P2pManager::local_endpoint()
        {
            return umgr_.local_endpoint();
        }

        P2pTunnel & P2pManager::get_tunnel(
            UdpEndpoint const & endpoint)
        {
            return umgr_.get_tunnel(endpoint);
        }

        P2pTunnel * P2pManager::get_tunnel(
            Uuid const & pid)
        {
            return umgr_.get_tunnel(pid);
        }

    } // namespace client
} // namespace trip
