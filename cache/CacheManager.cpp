// CacheManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/CacheManager.h"
#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/MemoryCachePool.h"
#include "trip/client/cache/DiskCachePool.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/main/MemoryManager.h"
#include "trip/client/utils/Format.h"
#include "trip/client/timer/TimerManager.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/system/HumanNumber.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CacheManager", framework::logger::Debug);

        CacheManager::CacheManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CacheManager>(daemon, "trip.client.CacheManager")
            , mmgr_(util::daemon::use_module<MemoryManager>(daemon))
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
            , capacity_(0)
            , cache_memory_(4 << 20) // 4M
            , memory_cache_(NULL)
            , disk_cache_(NULL)
        {
            module_config()
                << CONFIG_PARAM_NAME_RDWR("path", path_)
                << CONFIG_PARAM_NAME_RDWR("capacity", capacity_)
                << CONFIG_PARAM_NAME_RDWR("cache_memory", cache_memory_)
                ;
        }

        CacheManager::~CacheManager()
        {
        }

        bool CacheManager::startup(
            boost::system::error_code & ec)
        {
            LOG_INFO("capacity=" << capacity_ << ", cache_memory=" << cache_memory_);

            memory_cache_ = new MemoryCachePool(cache_memory_);
            if (capacity_) {
                disk_cache_ = new DiskCachePool(io_svc(), capacity_);
            }

            mmgr_.out_of_memory.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_added.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            tmgr_.t_1_s.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            tmgr_.t_10_s.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));

            io_svc().post(boost::bind(&CacheManager::load_cache, this));

            return true;
        }

        bool CacheManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_10_s.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            tmgr_.t_1_s.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_added.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            mmgr_.out_of_memory.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));

            delete memory_cache_;
            if (disk_cache_)
                delete disk_cache_;

            return true;
        }

        void CacheManager::load_cache()
        {
            boost::system::error_code ec;
            boost::filesystem::directory_iterator iter(path_, ec);
            if (ec) {
                LOG_WARN("[load_cache] ec=" << ec.message());
                return;
            }
            boost::filesystem::directory_iterator end;
            Uuid id;
            for (; iter != end; ++iter) {
                std::string idstr = iter->path().leaf().string();
                if (!id.from_string(idstr) && boost::filesystem::is_regular_file(iter->path() / "meta.xml")) {
                    LOG_INFO("[load_cache] found resource, id=" << id.to_string());
                    rmgr_.get(id);
                }
            }
        }

        void CacheManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == rmgr_) {
                Resource & r = *rmgr_.resource_added.resource;
                if (event == rmgr_.resource_added) {
                    //LOG_INFO("[on_event] resource_added, id=" << r.id());
                    ResourceCache * rcache = new ResourceCache(r, path_);
                    rcaches_[r.id()] = rcache;
                    ((DiskCachePool *)disk_cache_)->load_cache(rcache);
                    r.data_miss.on(
                        boost::bind(&CacheManager::on_event, this, _1, _2));
                    r.segment_full.on(
                        boost::bind(&CacheManager::on_event, this, _1, _2));
                } else if (event == rmgr_.resource_deleting) {
                    //LOG_INFO("[on_event] resource_deleting, id=" << r.id());
                    r.segment_full.un(
                        boost::bind(&CacheManager::on_event, this, _1, _2));
                    std::map<Uuid,  ResourceCache *>::iterator iter = 
                        rcaches_.find(r.id());
                    if (iter != rcaches_.end()) {
                        memory_cache_->free_cache(iter->second);
                        if (disk_cache_)
                            disk_cache_->free_cache(iter->second);
                        delete iter->second;
                        rcaches_.erase(iter);
                    }
                }
            } else if (observable == tmgr_) {
                if (event == tmgr_.t_1_s) {
                    memory_cache_->check_caches();
                } else {
                    if (disk_cache_)
                        disk_cache_->check_caches();
                    std::map<Uuid,  ResourceCache *>::iterator iter = rcaches_.begin();
                    for (; iter != rcaches_.end(); ++iter) {
                        iter->second->save_status();
                    }
                }
            } else if (observable == mmgr_) {
                    LOG_INFO("[on_event] out_of_memory, level=" << mmgr_.out_of_memory.level);
                    // for level 0, delete blocks not visited in current 30 seconds
                    // for level 5, delete blocks not visited in current 05 seconds
                    memory_cache_->reclaim_caches((6 - mmgr_.out_of_memory.level) * 5);
            } else {
                Resource const & r = (Resource const &)observable;
                ResourceCache * rcache = rcaches_[r.id()];
                if (event == r.segment_full) {
                    ResourceDataEvent const & e(r.segment_full);
                    //LOG_INFO("[on_event] segment_full, rid=" << r.id() << ", segment=" << e.id.segment);
                    if (disk_cache_) {
                        disk_cache_->alloc_cache(rcache, e.id, 0);
                    }
                } else if (event == r.data_miss) {
                    ResourceDataEvent const & e(r.data_miss);
                    //LOG_INFO("[on_event] data_miss, rid=" << r.id() << ", id=" << e.id);
                    memory_cache_->alloc_cache(rcache, e.id, e.degree * 5);
                }
            }
        }

    } // namespace client
} // namespace trip
