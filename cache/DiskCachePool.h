// DiskCachePool.h

#ifndef _TRIP_CLIENT_CACHE_DISK_CACHE_POOL_H_
#define _TRIP_CLIENT_CACHE_DISK_CACHE_POOL_H_

#include "trip/client/cache/CachePool.h"

namespace trip
{
    namespace client
    {

        class WorkQueue;

        class DiskCachePool
            : public CachePool
        {
        public:
            DiskCachePool(
                boost::asio::io_service & io_svc, 
                boost::uint64_t size);

            virtual ~DiskCachePool();

            void load_cache(
                ResourceCache * rcache);

        protected:
            virtual void const * alloc(
                Cache * cache, 
                DataId & from, 
                DataId & to);

            virtual bool check(
                Cache * cache); // return true if dirty

            virtual bool free(
                Cache * cache);

        private:
            bool do_save(
                Cache * cache, 
                DataId sid);

            void on_save(
                Cache * cache,
                DataId sid, 
                bool result);

        private:
            template <typename Archive>
            friend void serialize(
                Archive & ar, 
                DiskCachePool & t);

        private:
            WorkQueue * queue_;
            boost::uint64_t total_; // in blocks
            boost::uint64_t used_;
            std::vector<Cache> caches2_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_DISK_CACHE_POOL_H_
