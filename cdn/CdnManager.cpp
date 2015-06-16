// CdnManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/CdnFinder.h"
#include "trip/client/cdn/CdnTunnel.h"

namespace trip
{
    namespace client
    {

        CdnManager::CdnManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CdnManager>(daemon, "CdnManager")
            , finder_(new CdnFinder(*this))
        {
        }

        CdnManager::~CdnManager()
        {
            std::map<std::string, CdnTunnel *>::iterator iter = tunnels_.begin();
            for (; iter != tunnels_.end(); ++iter)
                delete iter->second;
            delete finder_;
        }

        bool CdnManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool CdnManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        CdnTunnel & CdnManager::get_tunnel(
            Url const & url)
        {
            std::string host_svc(url.host_svc());
            std::map<std::string, CdnTunnel *>::iterator iter = tunnels_.find(host_svc);
            if (iter == tunnels_.end()) {
                iter = tunnels_.insert(std::make_pair(host_svc, new CdnTunnel(io_svc()))).first;
            }
            return *iter->second;
        }

    } // namespace client
} // namespace trip
