// MemoryCachePool.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/MemoryCachePool.h"
#include "trip/client/cache/ResourceCache.h"

namespace trip
{
    namespace client
    {

        MemoryCachePool::MemoryCachePool(
            boost::uint32_t size)
            : CachePool(size / BLOCK_SIZE)
        {
        }

        void const * MemoryCachePool::alloc(
            Cache * cache, 
            DataId & from, 
            DataId & to)
        {
            Block const * blk = cache->rcache->map_block(from);
            if (blk) {
                from.inc_block(0);
                to = from;
                to.inc_block();
            }
            return blk;
        }

        bool MemoryCachePool::check(
            Cache * cache)
        {
            DataId from, to;
            Resource & r(cache->rcache->resource());
            r.query_lock(cache->lock, from, to);
            Block const * blk = r.get_segment(from)->get_block(from);
            assert(blk == cache->block);
            return cache->block->touched();
        }

        bool MemoryCachePool::free(
            Cache * cache)
        {
            return true;
        }

    } // namespace client
} // namespace trip
