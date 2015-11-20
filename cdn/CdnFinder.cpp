// CdnFinder.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnFinder.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/Error.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/core/Resource.h"

#include <util/archive/XmlIArchive.h>
#include <util/serialization/NVPair.h>
#include <util/serialization/stl/vector.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CdnFinder", framework::logger::Debug);

        CdnFinder::CdnFinder(
            CdnManager & cmgr)
            : url_("http://jump.trip.com/jump.xml")
            , cmgr_(cmgr)
            , http_(cmgr.io_svc())
        {
            Bootstrap::instance(cmgr.io_svc()).ready.on(
                boost::bind(&CdnFinder::on_event, this));
        }

        CdnFinder::~CdnFinder()
        {
        }

        std::string CdnFinder::proto() const
        {
            return "http";
        }

        void CdnFinder::on_event()
        {
            Bootstrap::instance(http_.get_io_service()).get("jump", url_);
        }

        void CdnFinder::find(
            Resource & resource, 
            size_t count)
        {
            LOG_INFO("[find] rid=" << resource.id());
            Url url(url_);
            url.param("rid", resource.id().to_string());
            std::vector<Url> const & urls = resource.urls();
            for (size_t i = 0; i < urls.size(); ++i) {
                url.param_add("url", urls[i].to_string());
            }
            url.param("count", framework::string::format(count));
            http_.async_fetch(url, 
                boost::bind(&CdnFinder::handle_fetch, this, _1));
        }

        Source * CdnFinder::create(
            Resource & resource, 
            Url const & url)
        {
            LOG_INFO("[create] rid=" << resource.id() << ", url=" << url.to_string());
            return new CdnSource(cmgr_.get_tunnel(url), resource, url);
        }

        void CdnFinder::handle_fetch(
            boost::system::error_code ec)
        {
            Url url("http://jump.trip.com" + http_.request_head().path);
            LOG_INFO("[handle_fetch] rid=" << url.param("rid") << ", ec=" << ec.message());
            Uuid rid(url.param("rid"));
            std::vector<Url> urls;
            if (!ec) {
                util::archive::XmlIArchive<> ia(http_.response_data());
                ia >> urls;
                if (!ia) {
                    ec = cdn_error::xml_format_error;
                }
            }
            found(rid, urls);
        }

    } // namespace client
} // namespace trip
