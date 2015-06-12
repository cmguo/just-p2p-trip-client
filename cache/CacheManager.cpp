// CacheManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/CacheManager.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/utils/Format.h"
#include "trip/client/timer/TimerManager.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        CacheManager::CacheManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CacheManager>(daemon, "CacheManager")
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
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
            rmgr_.resource_added.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            tmgr_.t_1_s.on(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            return true;
        }

        bool CacheManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_1_s.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            rmgr_.resource_added.un(
                boost::bind(&CacheManager::on_event, this, _1, _2));
            return true;
        }

        void CacheManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == rmgr_) {
                Resource & r = *rmgr_.resource_added.resource;
                if (event == rmgr_.resource_added) {
                    rcaches_[r.id()] = new ResourceCache(*this, r);
                } else {
                    std::map<Uuid,  ResourceCache *>::iterator iter = 
                        rcaches_.find(r.id());
                    if (iter != rcaches_.end()) {
                        delete iter->second;
                        rcaches_.erase(iter);
                        free_cache(r);
                    }
                }
            } else if (observable == tmgr_) {
            }
        }

        bool CacheManager::alloc_cache(
            Resource & resc,
            DataId const & block, 
            boost::filesystem::path const & path)
        {
            Cache ** p = alloc_cache();
            if (p == NULL || *p == NULL)
                return false;
            Cache * c = *p;
            c->resc = &resc;
            c->block = resc.map_block(block, path);
            if (c->block == NULL) {
                free_cache(p);
                return false;
            }
            DataId f = block;
            f.piece = 0;
            DataId t = f;
            t.top_segment_block++;
            c->lock = resc.alloc_lock(f, t);
            return true;
        }

        void CacheManager::free_cache(
            Resource & resc)
        {
            Cache ** p = &used_caches_;
            while (*p) {
                Cache * c = *p;
                if (c->resc == &resc) {
                    free_cache(p);
                } else {
                    p = &c->next;
                }
            }
        }

        CacheManager::Cache ** CacheManager::alloc_cache()
        {
            if (free_caches_ == NULL) {
                if (used_caches_ && used_caches_->lru > 0) {
                    free_cache(&used_caches_);
                }
            }
            if (free_caches_ == NULL) {
                return NULL;
            }
            Cache * c = free_caches_;
            free_caches_ = free_caches_->next;
            Cache ** p = used_caches_tail_;
            *used_caches_tail_ = c;
            used_caches_tail_ = &c->next;
            return p;
        }

        void CacheManager::free_cache(
            Cache ** cache)
        {
            Cache * c = *cache;
            if (c->lock)
                c->resc->release_lock(c->lock);
            c->resc = NULL;
            c->block = NULL;
            c->lru = 0;
            *cache = (*cache)->next;
            c->next = free_caches_;
            free_caches_ = c;
        }

        void CacheManager::check_caches()
        {
            Cache ** p = &used_caches_;
            for (; *p; p = &(*p)->next) {
                Cache * c = *p;
                if (c->block->touched()) {
                    c->lru = 0;
                    *p = c->next;
                    c->next = NULL;
                    *used_caches_tail_ = c;
                    used_caches_tail_ = &c->next;
                } else {
                    ++c->lru;
                }
            }
        }

        void CacheManager::reclaim_caches(
            size_t limit)
        {
            while (used_caches_) {
                if (used_caches_->lru < limit)
                    break;
                free_cache(&used_caches_);
            }
        }

    } // namespace client
} // namespace trip
