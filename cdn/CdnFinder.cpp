// CdnFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnFinder.h"
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

        void CdnFinder::find_more(
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
                boost::bind(&CdnFinder::handle_fetch, this, boost::ref(resource), _1));
        }

        void CdnFinder::handle_fetch(
            Resource & resource, 
            boost::system::error_code ec)
        {
            if (ec)
                return;

            util::archive::XmlIArchive<> ia(http_.response_data());
            std::vector<Url> urls;
            ia >> urls;
            found(resource, urls);
        }

    } // namespace client
} // namespace trip
