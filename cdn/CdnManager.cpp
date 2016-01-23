// CdnManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/CdnFinder.h"

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

    } // namespace client
} // namespace trip
