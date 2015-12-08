// DiskCachePool.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/DiskCachePool.h"
#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/WorkQueue.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.DiskCachePool", framework::logger::Debug);

        DiskCachePool::DiskCachePool(
            boost::asio::io_service & io_svc, 
            boost::uint64_t size)
            : CachePool((size_t)(size / SEGMENT_SIZE * 2))
            , queue_(new WorkQueue(io_svc))
        {
            total_ = size / BLOCK_SIZE;
            used_ = 0;
        }

        DiskCachePool::~DiskCachePool()
        {
            delete queue_;
        }

        void DiskCachePool::load_cache(
            ResourceCache * rcache)
        {
            boost::dynamic_bitset<boost::uint32_t> map;
            rcache->load_status(map);
            Cache ** p = NULL;
            DataId sid;
            Resource & res(rcache->resource());
            for (size_t i = 0; i < map.size(); ++i) {
                if (map[i]) {
                    LOG_INFO("[load_cache] found segment, rid=" << res.id().to_string() << ", segment=" << i);
                    p = alloc_cache(0);
                    if (p && *p) {
                        (*p)->rcache = rcache;
                        sid.block_piece = res.get_segment2(sid)->block_count();
                        (*p)->segid = sid.id;
                        used_ += sid.block_piece;
                    }
                    res.set_segment_status(sid, true);
                }
                sid.inc_segment();
            }
        }

        void const * DiskCachePool::alloc(
            Cache * cache, 
            DataId & from, 
            DataId & to)
        {
            Segment2 const * seg = cache->rcache->resource().get_segment2(from);
            if (total_ < used_ + seg->seg->block_count())
                seg = NULL;
            if (seg) {
                from.inc_segment(0);
                to = from;
                to.inc_segment();
                used_ += seg->block_count();
                if (cache_empty() && caches2_.empty()) {
                    if (used_ < total_) {
                        caches2_.resize(total_ / SEGMENT_SIZE * 2 * (total_ - used_) / used_);
                        for (size_t i = 0; i < caches2_.size(); ++i) {
                            free_cache(&caches2_[i]);
                        }
                    }
                }
                queue_->post(
                    boost::bind(&DiskCachePool::do_save, this, cache, from), 
                    boost::bind(&DiskCachePool::on_save, this, cache, from, _1));
            }
            return seg;
        }

        bool DiskCachePool::check(
            Cache * cache)
        {
            return true;
        }

        bool DiskCachePool::free(
            Cache * cache)
        {
            used_ -= cache->segment->block_count();
            return true;
        }

        bool DiskCachePool::do_save(
            Cache * cache, 
            DataId sid)
        {
            return cache->rcache->save_segment(sid, *cache->segment);
        }

        void DiskCachePool::on_save(
            Cache * cache, 
            DataId sid, 
            bool result)
        {
            if (result) {
                cache->data = NULL;
                cache->rcache->resource().set_segment_status(sid, true);
                cache->rcache->resource().release_lock(cache->lock);
                Resource & res(cache->rcache->resource());
                sid.block_piece = res.get_segment2(sid)->block_count();
                cache->segid = sid.id;
            } else {
                free_cache(cache);
            }
        }

    } // namespace client
} // namespace trip
