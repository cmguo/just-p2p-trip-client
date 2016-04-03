// Resource.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_H_
#define _TRIP_CLIENT_CORE_RESOURCE_H_

#include "trip/client/core/ResourceData.h"

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        struct ResourceMeta
        {
            boost::uint64_t duration;
            boost::uint64_t bytesize;
            boost::uint32_t interval;
            boost::uint32_t bitrate;
            boost::uint64_t segcount;
            std::string file_extension;
        };

        struct ResourceStat
        {
        };

        class DownloadInfo;

        class Resource
            : public ResourceData
        {
        public:
            Resource();

            ~Resource();

        public:
            //ResourceErrorEvent error_occurred;

            //ResourceMetaEvent meta_changed;

        public:
            framework::string::Uuid const & id() const
            {
                return id_;
            }

            ResourceMeta const * meta() const
            {
                return meta_;
            }

        public:
            bool load(
                boost::filesystem::path const & path, 
                boost::system::error_code & ec);

        public:
            void set_id(
                Uuid const & id);

            void set_meta(
                ResourceMeta const & meta);

        private:
            Resource(Resource const &);

        private:
            Uuid id_;
            ResourceMeta const * meta_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_H_

