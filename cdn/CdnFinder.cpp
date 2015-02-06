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
            Resource & resource)
        {
            Url url("http://index.trip.com/map.htm");
            if (!resource.id().is_empty())
                url.param("rid", "resource.id().to_string()");
            
            http_.async_fetch(url, 
                boost::bind(&CdnFinder::handle_fetch, this, _1));
        }

        void CdnFinder::handle_fetch(
            boost::system::error_code ec)
        {
            if (ec)
                return;

            util::archive::XmlIArchive<> ia(http_.response_data());
            ResourceInfo info;
            ia >> info;
        }

    } // namespace client
} // namespace trip
