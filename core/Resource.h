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
            ResourceChangedEvent merged;

            MetaChangedEvent meta_changed;

            SourceChangedEvent source_changed;

            SinkChangedEvent sink_changed;

        public:
            framework::string::Uuid const & id() const
            {
                return id_;
            }

            ResourceMeta const * meta() const
            {
                return meta_;
            }

            std::vector<Url> const & urls() const
            {
                return urls_;
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

            void merge(
                Resource & other);

            void set_urls(
                std::vector<Url> & urls);

        public:
            void add_sink(
                Sink * sink);

            void del_sink(
                Sink * sink);

            void update_sink(
                Sink * sink);

            std::vector<Sink *> & get_sinks()
            {
                return sinks_;
            }

        private:
            Resource(Resource const &);

        private:
            Uuid id_;
            ResourceMeta const * meta_;
            std::vector<Url> urls_;
            std::vector<Sink *> sinks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_H_

