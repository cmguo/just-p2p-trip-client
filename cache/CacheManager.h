// CacheManager.h

#ifndef _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
#define _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_

#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/WorkQueue.h"

#include "trip/client/core/Resource.h"

#include <util/event/Event.h>

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class ResourceManager;
        class TimerManager;

        class CacheManager
            : public util::daemon::ModuleBase<CacheManager>
        {
        public:
            CacheManager(
                util::daemon::Daemon & daemon);

            ~CacheManager();

        public:
            struct Cache
            {
                Cache * next;
                Resource *resc;
                Block const * block;
                Resource::lock_t lock;
                size_t lru;
            };

            bool alloc_cache(
                Resource & resc, 
                DataId const & block, 
                boost::filesystem::path const & path);

            void free_cache(
                Resource & resc);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

        private:
            Cache ** alloc_cache();

            void free_cache(
                Cache ** cache);

            void check_caches();

            void reclaim_caches(
                size_t limit);

        private:
            ResourceManager & rmgr_;
            TimerManager & tmgr_;
            boost::filesystem::path path_;
            boost::uint64_t capacity_;
            std::map<Uuid, ResourceCache *> rcaches_;
            WorkQueue queue_;
            Cache * used_caches_;
            Cache ** used_caches_tail_;
            Cache * free_caches_;
            std::vector<Cache> caches_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
