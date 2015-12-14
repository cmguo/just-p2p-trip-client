// CachePool.h

#ifndef _TRIP_CLIENT_CACHE_CACHE_POOL_H_
#define _TRIP_CLIENT_CACHE_CACHE_POOL_H_

#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        class ResourceCache;

        struct Cache
        {
            Cache * next;
            ResourceCache * rcache;
            union {
                void const * data;
                Block const * block; // memory cache
                Segment2 const * segment; // disk cache
            };
            union {
                void const * data2;
                Resource::lock_t lock; // memory cache
                boost::uint64_t segid; // disk cache, segment, use block_piece from block count 
            };
            size_t lru;

            Cache()
                : next(NULL)
                , rcache(NULL)
                , data(NULL)
                , lock(NULL)
                , lru(0)
            {
            }
        };

        class CachePool
        {
        public:
            CachePool(
                size_t size);

            virtual ~CachePool();

        public:
            bool alloc_cache(
                ResourceCache * rcache, 
                DataId const & block, 
                size_t level);

            void free_cache(
                ResourceCache * rcache);

            void check_caches();

            void reclaim_caches(
                size_t limit);

        protected:
            Cache ** alloc_cache(
                size_t level);

            void free_cache(
                Cache ** cache);

            void free_cache(
                Cache * cache);

            bool cache_empty() const
            {
                return free_caches_ == NULL;
            }

        protected:
            virtual void const * alloc(
                Cache * cache, 
                DataId & from, 
                DataId & to) = 0;

            virtual bool check(
                Cache * cache) = 0; // return true if dirty

            virtual bool free(
                Cache * cache) = 0;

        private:
            Cache * used_caches_;
            Cache ** used_caches_tail_;
            Cache * free_caches_;
            std::vector<Cache> caches_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_CACHE_POOL_H_
