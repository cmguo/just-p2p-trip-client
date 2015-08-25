// CachePool.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/CachePool.h"
#include "trip/client/cache/ResourceCache.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CachePool", framework::logger::Debug);

        CachePool::CachePool(
            size_t size)
            : used_caches_(NULL)
            , used_caches_tail_(&used_caches_)
        {
            caches_.resize(size);
            free_caches_ = &caches_[0];
            for (size_t i = 1; i < size; ++i) {
                free_caches_->next = &caches_[i];
                free_caches_ = &caches_[i];
            }
            free_caches_->next = NULL;
            free_caches_ = &caches_[0];
        }

        CachePool::~CachePool()
        {
        }

        bool CachePool::alloc_cache(
            ResourceCache * rcache,
            DataId const & id)
        {
            Cache ** p = alloc_cache();
            if (p == NULL || *p == NULL) {
                LOG_WARN("[alloc_cache] no cache!");
                return false;
            }
            Cache * c = *p;
            c->rcache = rcache;
            DataId f = id;
            DataId t;
            c->data = alloc(c, f, t);
            if (c->data == NULL) {
                LOG_WARN("[alloc_cache] failed!");
                free_cache(p);
                return false;
            }
            c->lock = rcache->resource().alloc_lock(f, t);
            return true;
        }

        void CachePool::free_cache(
            ResourceCache * rcache)
        {
            Cache ** p = &used_caches_;
            while (*p) {
                Cache * c = *p;
                if (c->rcache == rcache) {
                    free_cache(p);
                } else {
                    p = &c->next;
                }
            }
        }

        void CachePool::check_caches()
        {
            Cache ** p = &used_caches_;
            for (; *p; p = &(*p)->next) {
                Cache * c = *p;
                if (check(c)) {
                    c->lru = 0;
                } else {
                    ++c->lru;
                }
            }
        }

        void CachePool::reclaim_caches(
            size_t limit)
        {
            while (used_caches_) {
                if (used_caches_->lru < limit)
                    break;

                free_cache(&used_caches_);
            }
        }

        Cache ** CachePool::alloc_cache()
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
            *used_caches_tail_ = NULL;
            return p;
        }

        void CachePool::free_cache(
            Cache ** cache)
        {
            Cache * c = *cache;
            free(c);
            if (c->lock)
                c->rcache->resource().release_lock(c->lock);
            c->rcache = NULL;
            c->data = NULL;
            c->lru = 0;
            *cache = (*cache)->next;
            if (&c->next == used_caches_tail_)
                used_caches_tail_ = cache;
            c->next = free_caches_;
            free_caches_ = c;
        }

        void CachePool::free_cache(
            Cache * cache)
        {
            Cache ** p = &used_caches_;
            while (*p) {
                if (*p == cache) {
                    free_cache(p);
                    return;
                }
                p = &(*p)->next;
            }
        }

    } // namespace client
} // namespace trip
