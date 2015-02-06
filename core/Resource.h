// Resource.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_H_
#define _TRIP_CLIENT_CORE_RESOURCE_H_

#include "trip/client/core/ResourceData.h"

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        class Source;

        struct ResourceMeta
        {
            boost::uint64_t duration;
            boost::uint32_t interval;
            std::string file_extension;
        };

        class Resource
            : public ResourceData
        {
        public:
            Resource();

            ~Resource();

        public:
            MetaChangedEvent meta_changed;

            SourceChangedEvent source_changed;

            SinkChangedEvent sink_changed;

        public:
            framework::string::Uuid const & id() const
            {
                return id_;
            }

            ResourceMeta const & meta() const
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

            void set_urls(
                std::vector<Url> const & urls);

        private:
            Uuid id_;
            ResourceMeta meta_;
            std::vector<Url> urls_;
            std::vector<Source *> sources_;
            std::vector<Sink *> sinks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_H_

