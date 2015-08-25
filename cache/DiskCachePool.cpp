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
            size_t size)
            : CachePool(size)
            , queue_(new WorkQueue(io_svc))
        {
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
                    p = alloc_cache();
                    if (p && *p) {
                        (*p)->rcache = rcache;
                        (*p)->seg_num = i;
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
            if (seg) {
                from.inc_segment(0);
                to = from;
                to.inc_segment();
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
                cache->seg_num = sid.segment;
            } else {
                free_cache(cache);
            }
        }

    } // namespace client
} // namespace trip
