// MemoryCachePool.h

#ifndef _TRIP_CLIENT_CACHE_MEMORY_CACHE_POOL_H_
#define _TRIP_CLIENT_CACHE_MEMORY_CACHE_POOL_H_

#include "trip/client/cache/CachePool.h"

namespace trip
{
    namespace client
    {

        class MemoryCachePool
            : public CachePool
        {
        public:
            MemoryCachePool(
                boost::uint32_t size);

        protected:
            virtual void const * alloc(
                Cache * cache, 
                DataId & from, 
                DataId & to);

            virtual bool check(
                Cache * cache); // return true if dirty

            virtual bool free(
                Cache * cache);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_MEMORY_CACHE_POOL_H_
