// Serialize.h

#ifndef _TRIP_CLIENT_CACHE_SERIALIZE_H_
#define _TRIP_CLIENT_CACHE_SERIALIZE_H_

#include "trip/client/cache/CacheManager.h"
#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/DiskCachePool.h"
#include "trip/client/cache/MemoryCachePool.h"
#include "trip/client/core/Serialize.h"
#include "trip/client/utils/Serialize.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/map.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceCache & t)
        {
            ar & SERIALIZATION_NVP_2(t, directory)
                & SERIALIZATION_NVP_2(t, is_external);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Cache & t)
        {
            ar & SERIALIZATION_NVP_1(t, level)
                & SERIALIZATION_NVP_1(t, lru)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            CachePool & t)
        {
            if (t.used_caches_)
            ar & SERIALIZATION_NVP_3(t, used_caches);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            MemoryCachePool & t)
        {
            serialize(ar, (CachePool &)t);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            DiskCachePool & t)
        {
            ar & SERIALIZATION_NVP_3(t, total)
                & SERIALIZATION_NVP_3(t, used);
            serialize(ar, (CachePool &)t);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            CacheManager & t)
        {
            ar & SERIALIZATION_NVP_3(t, path)
                & SERIALIZATION_NVP_3(t, capacity)
                & SERIALIZATION_NVP_3(t, cache_memory)
                & SERIALIZATION_NVP_3(t, rcaches)
                & SERIALIZATION_NVP_NAME("memory_cache", (MemoryCachePool *)t.memory_cache_)
                & SERIALIZATION_NVP_NAME("disk_cache", (DiskCachePool *)t.disk_cache_)
                ;
        }

    }
}

namespace util
{
    namespace serialization
    {

        template <class Archive>
        struct is_single_unit<Archive, trip::client::MemoryCachePool>
            : boost::true_type
        {
        };

    }
}

#endif // _TRIP_CLIENT_CACHE_SERIALIZE_H_
