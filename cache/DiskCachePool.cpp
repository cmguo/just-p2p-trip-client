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
            DataId sid;
            sid.top = 1;
            for (size_t i = 0; i < map.size(); ++i) {
                if (map[i]) {
                    alloc_cache(rcache, sid, 0);
                }
                sid.inc_segment();
            }
        }

        void const * DiskCachePool::alloc(
            Cache * cache, 
            DataId & from, 
            DataId & to)
        {
            // from 
            Segment2 const * seg = cache->rcache->resource().get_segment2(from);
            if (total_ < used_ + seg->block_count() && from.top == 0 && !cache->rcache->is_external()) {
                seg = NULL;
            }
            DataId sid(from);
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
                    boost::bind(&DiskCachePool::do_save, this, cache, sid), 
                    boost::bind(&DiskCachePool::on_save, this, cache, sid, _1));
            }
            return seg;
        }

        bool DiskCachePool::check(
            Cache * cache)
        {
            if (cache->data != this) // save not finished
                return true;
            return true;
        }

        bool DiskCachePool::free(
            Cache * cache)
        {
            if (cache->data != this) // save not finished
                return false;
            used_ -= DataId(cache->segid).block_piece;
            cache->rcache->free_segment(cache->segid);
            return true;
        }

        bool DiskCachePool::do_save(
            Cache * cache, 
            DataId sid)
        {
            if (sid.top) { // load
                return cache->rcache->load_segment(sid, *cache->segment->meta);
            } else {
                return cache->rcache->save_segment(sid, *cache->segment);
            }
        }

        void DiskCachePool::on_save(
            Cache * cache, 
            DataId sid, 
            bool result)
        {
            Resource & res(cache->rcache->resource());
            Segment2 const * seg = res.get_segment2(sid);
            sid.top = 0;
            sid.block_piece = seg->block_count();
            if (result)
                res.set_segment_status(sid, true);
            if (result && !cache->rcache->is_external()) {
                cache->data = this;
                res.release_lock(cache->lock);
                cache->segid = sid.id;
            } else {
                used_ -= sid.block_piece;
                free_cache(cache);
            }
        }

    } // namespace client
} // namespace trip
