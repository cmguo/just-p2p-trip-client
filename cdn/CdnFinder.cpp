// CdnFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnFinder.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/Error.h"
#include "trip/client/proto/MessageIndex.h"
#include "trip/client/core/Resource.h"

#include <util/archive/XmlIArchive.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CdnFinder", framework::logger::Debug);

        CdnFinder::CdnFinder(
            CdnManager & cmgr)
            : cmgr_(cmgr)
            , http_(cmgr.io_svc())
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
            LOG_INFO("[find] rid=" << resource.id());
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
            LOG_INFO("[create] rid=" << resource.id() << ", url=" << url.to_string());
            return new CdnSource(cmgr_.get_tunnel(url), resource, url);
        }

        void CdnFinder::handle_fetch(
            boost::system::error_code ec, 
            Resource & resource)
        {
             LOG_INFO("[handle_fetch] rid=" << resource.id() << ", ec=" << ec.message());
            std::vector<Url> urls;
            if (!ec) {
                util::archive::XmlIArchive<> ia(http_.response_data());
                ia >> urls;
                if (!ia) {
                    ec = cdn_error::xml_format_error;
                }
            }
            found(ec, resource, urls);
        }

    } // namespace client
} // namespace trip
