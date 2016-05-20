// ResourceManager.cpp

#include "trip/client/Common.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/main/Bootstrap.h"
#include "trip/client/proto/MessageResource.h"
#include "trip/client/cdn/Error.h"
#include "trip/client/utils/Format.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/ResourceEvent.hpp"

#include <util/event/EventEx.hpp>
#include <util/archive/XmlIArchive.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.ResourceManager", framework::logger::Debug);

        ResourceManager::ResourceManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<ResourceManager>(daemon, "trip.client.ResourceManager")
            , util::event::Observable("trip.client.ResourceManager")
            , resource_added("resource_added")
            , resource_deleting("resource_deleting")
            , url_("http://index.trip.com/trip/resource.xml")
            , http_(io_svc())
        {
            register_event(resource_added);
            register_event(resource_deleting);
            Bootstrap::instance(io_svc()).ready.on(
                boost::bind(&ResourceManager::on_event, this));
        }

        ResourceManager::~ResourceManager()
        {
        }

        bool ResourceManager::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool ResourceManager::shutdown(
            boost::system::error_code & ec)
        {
            http_.cancel_forever(ec);
            return true;
        }

        void ResourceManager::dump() const
        {
            std::map<Uuid, Resource *>::const_iterator iter = 
                resources_.begin();
            for (; iter != resources_.end(); ++iter) {
                Resource const & res(*iter->second);
                LOG_INFO("[dump] [resouce] " << res.id());
            }
        }

        Resource * ResourceManager::find(
            Uuid const & rid)
        {
            std::map<Uuid, Resource *>::iterator iter = 
                resources_.find(rid);
            if (iter == resources_.end()) {
                return NULL;
            } else {
                return iter->second;
            }
        }

        Resource & ResourceManager::get(
            Uuid const & rid)
        {
            std::map<Uuid, Resource *>::iterator iter = 
                resources_.find(rid);
            if (iter == resources_.end()) {
                Resource * r = new Resource;
                iter = resources_.insert(std::make_pair(rid, r)).first;
                resource_added.resource = r;
                r->set_id(rid);
                raise(resource_added);
            }
            return *iter->second;
        }

        void ResourceManager::async_get(
            std::vector<Url> & urls, 
            ResourceEvent & event)
        {
            for (size_t i = 0; i < urls.size(); ++i) {
                if (urls[i].protocol() == "rid") {
                    Uuid rid(urls[i].path().substr(1));
                    Resource & resource = get(rid);
                    event.resource = &resource;
                    raise(event);
                    return;
                }
            }
            bool empty = async_requests_.empty();
            async_requests_.push_back(std::make_pair(urls, &event));
            if (!empty) return;
            get_next();
        }

        void ResourceManager::get_urls(
            Uuid const & rid, 
            std::vector<Url> & urls)
        {
            std::map<Uuid, std::vector<Url> >::iterator iter = 
                resource_urls_.find(rid);
            if (iter != resource_urls_.end()) {
                urls.swap(iter->second);
                resource_urls_.erase(iter);
            }
        }

        void ResourceManager::on_event()
        {
            Bootstrap::instance(http_.get_io_service()).get("index", url_);
        }

        void ResourceManager::get_next()
        {
            Url url(url_);
            std::vector<Url> & urls = async_requests_.front().first;
            for (size_t i = 0; i < urls.size(); ++i) {
                url.param("url", urls[i].to_string());
            }
            http_.async_fetch(url, 
                boost::bind(&ResourceManager::handle_fetch, this, _1));
        }

        void ResourceManager::handle_fetch(
            boost::system::error_code ec)
        {
            ResourceEvent & event = *async_requests_.front().second;
            async_requests_.pop_front();

            ResourceInfo info;
            if (!ec) {
                util::archive::XmlIArchive<> ia(http_.response_data());
                ia >> info;
                if (!ia)
                    ec = cdn_error::xml_format_error;
            }

            if (!ec) {
                Resource & resource = get(info.id);
                if (info.urls.is_initialized())
                    resource_urls_[info.id].swap(info.urls.get());
                if (info.segments.is_initialized())
                    resource.set_segments(info.segments.get());
                resource.set_meta(info.meta);
                event.resource = &resource;
            }

            if (ec) {
                LOG_WARN("[handle_fetch] ec:" << ec.message());
                last_error_ = ec;
            }

            raise(event);

            if (!async_requests_.empty())
                get_next();
        }

    } // namespace client
} // namespace trip
