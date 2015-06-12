// ResourceCache.h

#ifndef _TRIP_CLIENT_CACHE_RESOURCE_CACHE_H_
#define _TRIP_CLIENT_CACHE_RESOURCE_CACHE_H_

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
                CacheManager & cmgr,
                Resource & resource);

            ~ResourceCache();

        public:
            bool load_status(
                boost::filesystem::path const & directory);

            bool load_piece(
                boost::uint64_t index);

        private:
            void on_event(
                util::event::Event const & event);

        private:
            bool save(
                boost::uint64_t segid);

            void on_saved(
                bool result);

        private:
            CacheManager & cmgr_;
            Resource & resource_;
            boost::filesystem::path directory_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CACHE_RESOURCE_CACHE_H_
