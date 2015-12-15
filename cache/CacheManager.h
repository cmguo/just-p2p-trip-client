// CacheManager.h

#ifndef _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
#define _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_

#include "trip/client/core/Resource.h"

#include <util/event/Event.h>
#include <util/daemon/Module.h>

#include <framework/system/HumanNumber.h>

namespace trip
{
    namespace client
    {

        class MemoryManager;
        class ResourceManager;
        class TimerManager;
        class CachePool;
        class ResourceCache;

        class CacheManager
            : public util::daemon::ModuleBase<CacheManager>
        {
        public:
            CacheManager(
                util::daemon::Daemon & daemon);

            ~CacheManager();

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void load_cache();

            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

        private:
            MemoryManager & mmgr_;
            ResourceManager & rmgr_;
            TimerManager & tmgr_;
            boost::filesystem::path path_;
            framework::system::HumanNumber<boost::uint64_t> capacity_;
            framework::system::HumanNumber<boost::uint32_t> cache_memory_;
            std::map<Uuid, ResourceCache *> rcaches_;
            CachePool * memory_cache_;
            CachePool * disk_cache_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_CACHE_MANAGER_H_
