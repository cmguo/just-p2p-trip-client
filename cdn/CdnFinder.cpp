// CdnFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnFinder.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/cdn/Error.h"
#include "trip/client/proto/MessageIndex.h"
#include "trip/client/core/Resource.h"

#include <util/archive/XmlIArchive.h>

namespace trip
{
    namespace client
    {

        CdnFinder::CdnFinder(
             boost::asio::io_service & io_svc)
            : http_(io_svc)
        {
        }

        CdnFinder::~CdnFinder()
        {
        }

        std::string CdnFinder::proto() const
        {
            return "http";
        }

        void CdnFinder::find(
            Resource & resource, 
            size_t count)
        {
            Url url("http://jump.trip.com/jump.xml");
            url.param("rid", resource.id().to_string());
            std::vector<Url> const & urls = resource.urls();
            for (size_t i = 0; i < urls.size(); ++i) {
                url.param_add("url", urls[i].to_string());
            }
            url.param("count", framework::string::format(count));
            http_.async_fetch(url, 
                boost::bind(&CdnFinder::handle_fetch, this, _1, boost::ref(resource)));
        }

        Source * CdnFinder::create(
            Resource & resource, 
            Url const & url)
        {
            return new CdnSource(http_.get_io_service(), resource, url);
        }

        void CdnFinder::handle_fetch(
            boost::system::error_code ec, 
            Resource & resource)
        {
            std::vector<Url> urls;
            if (!ec) {
                util::archive::XmlIArchive<> ia(http_.response_data());
                ia >> urls;
                if (!ia) {
                    ec = cdn_error::xml_format_error;
                }
            }
            found(resource, urls);
        }

    } // namespace client
} // namespace trip
