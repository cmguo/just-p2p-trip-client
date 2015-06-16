// P2pManager.cpp

#include "trip/client/Common.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/p2p/P2pFinder.h"
#include "trip/client/udp/UdpManager.h"

namespace trip
{
    namespace client
    {

        P2pManager::P2pManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<P2pManager>(daemon, "P2pManager")
            , finder_(new P2pFinder(util::daemon::use_module<UdpManager>(daemon)))
        {
        }

        P2pManager::~P2pManager()
        {
            delete finder_;
        }

        bool P2pManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool P2pManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

    } // namespace client
} // namespace trip
