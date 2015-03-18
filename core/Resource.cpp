// Resource.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Resource.h"

#include <algorithm>

namespace trip
{
    namespace client
    {

        Resource::Resource()
        {
        }

        Resource::~Resource()
        {
        }

        bool Resource::load(
            boost::filesystem::path const & path, 
            boost::system::error_code & ec)
        {
            return false;
        }

        void Resource::set_id(
            Uuid const & id)
        {
            id_ = id;
        }

        void Resource::set_meta(
            ResourceMeta const & meta)
        {
            meta_ = new ResourceMeta(meta);
            meta_changed.meta = meta_;
            raise(meta_changed);
        }

        void Resource::merge(
            Resource & other)
        {
            other.urls_.insert(other.urls_.end(),
                urls_.begin(), urls_.end());
            merged.resource = &other;
            raise(merged);
        }

        void Resource::set_urls(
            std::vector<Url> & urls)
        {
            std::vector<Url> urls2;
            for (size_t i = 0; i < urls.size(); ++i) {
                if (std::find(urls_.begin(), urls_.end(), urls[i]) == urls_.end())
                    urls2.push_back(urls[i]);
            }
            urls_.insert(urls_.end(), urls2.begin(), urls2.end());
            urls.swap(urls2);
        }

        void Resource::add_sink(
            Sink * sink)
        {
            sinks_.push_back(sink);
            sink_changed.type = 0;
            sink_changed.sink = sink;
            raise(sink_changed);
        }

        void Resource::del_sink(
            Sink * sink)
        {
            sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink));
            sink_changed.type = 1;
            sink_changed.sink = sink;
            raise(sink_changed);
        }

        void Resource::update_sink(
            Sink * sink)
        {
            sink_changed.type = 2;
            sink_changed.sink = sink;
            raise(sink_changed);
        }

    } // namespace client
} // namespace trip
