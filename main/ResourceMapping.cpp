// ResourceMapping.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ResourceMapping.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/proto/MessageResource.h"
#include "trip/client/core/Resource.h"

#include <util/archive/XmlIArchive.h>

namespace trip
{
    namespace client
    {

        ResourceMapping::ResourceMapping(
             boost::asio::io_service & io_svc)
            : url_("http://index.trip.com/trip/resource.xml")
            , http_(io_svc)
        {
            Bootstrap::instance(io_svc).ready.on(
                boost::bind(&ResourceMapping::on_event, this));
        }

        ResourceMapping::~ResourceMapping()
        {
        }

        void ResourceMapping::on_event()
        {
            Bootstrap::instance(http_.get_io_service()).get("index", url_);
        }

        void ResourceMapping::find(
            Resource & resource, 
            std::vector<Url> const & urls)
        {
            Url url(url_);
            for (size_t i = 0; i < urls.size(); ++i) {
                url.param("url", urls[i].to_string());
            }
            http_.async_fetch(url, 
                boost::bind(&ResourceMapping::handle_fetch, this, _1, boost::ref(resource)));
        }

        bool ResourceMapping::close()
        {
            boost::system::error_code ec;
            http_.cancel_forever(ec);
            return !ec;
        }

        void ResourceMapping::handle_fetch(
            boost::system::error_code ec, 
            Resource & resource)
        {
            if (ec)
                return;

            util::archive::XmlIArchive<> ia(http_.response_data());
            ResourceInfo info;
            ia >> info;

            resource.set_id(info.id);
            resource.set_meta(info.meta);
            if (info.urls.is_initialized())
                resource.set_urls(info.urls.get());
            if (info.segments.is_initialized())
                resource.set_segments(info.segments.get());
        }

    } // namespace client
} // namespace trip
