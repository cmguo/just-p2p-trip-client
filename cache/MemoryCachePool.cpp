// MemoryCachePool.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/MemoryCachePool.h"
#include "trip/client/cache/ResourceCache.h"

namespace trip
{
    namespace client
    {

        MemoryCachePool::MemoryCachePool(
            size_t size)
            : CachePool(size)
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
            return cache->block->touched();
        }

        bool MemoryCachePool::free(
            Cache * cache)
        {
            return true;
        }

    } // namespace client
} // namespace trip
