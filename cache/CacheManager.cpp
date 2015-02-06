// CacheManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/CacheManager.h"
#include "trip/client/utils/Format.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        CacheManager::CacheManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CacheManager>(daemon)
            , queue_(io_svc())
        {
            config().register_module("CacheManager")
                << CONFIG_PARAM_NAME_NOACC("path", path_)
                << CONFIG_PARAM_NAME_NOACC("capacity", capacity_);
        }

        CacheManager::~CacheManager()
        {
        }

        bool CacheManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool CacheManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

    } // namespace client
} // namespace trip
