// ResourceMapping.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ResourceMapping.h"
#include "trip/client/proto/MessageIndex.h"
#include "trip/client/core/Resource.h"

#include <util/archive/XmlIArchive.h>

namespace trip
{
    namespace client
    {

        ResourceMapping::ResourceMapping(
             boost::asio::io_service & io_svc)
            : http_(io_svc)
        {
        }

        ResourceMapping::~ResourceMapping()
        {
        }

        void ResourceMapping::find(
            Resource & resource)
        {
            Url url("http://index.trip.com/test.xml");
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

            resource.set_id(info.rid);
            ResourceMeta meta;
            meta.duration = info.duration;
            meta.interval = info.interval;
            resource.set_meta(meta);
        }

    } // namespace client
} // namespace trip
