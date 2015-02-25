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
            raise(meta_changed);
        }

        void Resource::set_meta(
            ResourceMeta const & meta)
        {
            meta_ = new ResourceMeta(meta);
            raise(meta_changed);
        }

        void Resource::set_urls(
            std::vector<Url> const & urls)
        {
            urls_ = urls;
            raise(meta_changed);
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
