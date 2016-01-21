// ResourceCache.h

#ifndef _TRIP_CLIENT_CACHE_RESOURCE_CACHE_H_
#define _TRIP_CLIENT_CACHE_RESOURCE_CACHE_H_

#include "trip/client/core/DataId.h"
#include "trip/client/core/Segment.h"

#include <util/event/Event.h>

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        class CacheManager;
        class Resource;

        class ResourceCache
        {
        public:
            ResourceCache(
                Resource & resource, 
                boost::filesystem::path const & directory);

            ~ResourceCache();

        public:
            Resource & resource()
            {
                return resource_;
            }

            boost::filesystem::path const & directory() const
            {
                return directory_;
            }

            bool is_external() const
            {
                return is_external_;
            }

        public:
            bool save_status();

            bool load_status(
                boost::dynamic_bitset<boost::uint32_t> & map);

            Block const * map_block(
                DataId bid);

            bool save_segment(
                DataId sid, 
                Segment2 const & segment);

            bool load_segment(
                DataId sid, 
                SegmentMeta const & segment);

            bool free_segment(
                DataId sid);

        private:
            boost::filesystem::path seg_path(
                DataId sid) const;

        private:
            Resource & resource_;
            boost::filesystem::path directory_;
            bool is_external_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_RESOURCE_CACHE_H_
