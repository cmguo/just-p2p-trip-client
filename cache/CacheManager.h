// CacheManager.h

#ifndef _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
#define _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_

#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/WorkQueue.h"

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class CacheManager
            : public util::daemon::ModuleBase<CacheManager>
        {
        public:
            CacheManager(
                util::daemon::Daemon & daemon);

            ~CacheManager();

        public:
            ResourceCache & get_cache(
                Uuid const & rid);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            boost::filesystem::path path_;
            boost::uint64_t capacity_;
            std::map<Uuid, ResourceCache> caches_;
            WorkQueue queue_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
