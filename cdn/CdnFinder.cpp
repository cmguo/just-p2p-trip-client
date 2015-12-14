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
            : url_("http://jump.trip.com/trip/jump.xml")
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
            std::vector<Url> urls;
            resource.set_urls(urls); // swap
            if (urls.empty()) {
                LOG_DEBUG("[find] rid=" << resource.id());
                Url url(url_);
                url.param("rid", resource.id().to_string());
                url.param("count", framework::string::format(count));
                http_.async_fetch(url, 
                    boost::bind(&CdnFinder::handle_fetch, this, _1));
            } else {
                http_.get_io_service().post(
                    boost::bind(&CdnFinder::found, this, resource.id(), urls));
            }
        }

        Source * CdnFinder::create(
            Resource & resource, 
            Url const & url)
        {
            LOG_DEBUG("[create] rid=" << resource.id() << ", url=" << url.to_string());
            return new CdnSource(cmgr_.get_tunnel(url), resource, url);
        }

        void CdnFinder::handle_fetch(
            boost::system::error_code ec)
        {
            Url url("http://jump.trip.com" + http_.request_head().path);
            Uuid rid(url.param("rid"));
            std::vector<Url> urls;
            if (!ec) {
                util::archive::XmlIArchive<> ia(http_.response_data());
                ia >> urls;
                if (!ia) {
                    ec = cdn_error::xml_format_error;
                }
            }
            if (!ec) {
                found(rid, urls);
            } else {
                LOG_DEBUG("[handle_fetch] rid=" << url.param("rid") << ", ec=" << ec.message());
            }
        }

    } // namespace client
} // namespace trip
