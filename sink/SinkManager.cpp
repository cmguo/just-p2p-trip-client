// SinkManager.cpp

#include "trip/client/Common.h"
#include "trip/client/sink/SinkManager.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/timer/TimerManager.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Sink.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/system/LogicError.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.SinkManager", framework::logger::Debug);

        SinkManager::SinkManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<SinkManager>(daemon, "SinkManager")
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
            , dmgr_(util::daemon::use_module<DownloadManager>(daemon))
        {
        }

        SinkManager::~SinkManager()
        {
        }

        bool SinkManager::startup(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.on(
                boost::bind(&SinkManager::on_event, this, _1, _2));
            return true;
        }

        bool SinkManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.un(
                boost::bind(&SinkManager::on_event, this, _1, _2));
            return true;
        }

        struct SinkManager::GetResourceEvent : ResourceEvent
        {
            Sink * sink;
            GetResourceEvent(Sink * s) : ResourceEvent("got_resource"), sink(s) {}
            struct equal
            {
                Sink * sink;
                equal(Sink * s) : sink(s) {}
                bool operator()(GetResourceEvent * e) const { return e->sink == sink; }
            };
        };

        void SinkManager::add_sink(
            Sink * sink)
        {
            LOG_INFO("[add_sink] sink:" << sink << ", url:" << sink->url().to_string());
            Url const & url(sink->url());
            std::vector<Url> urls;
            Url params("param:///");
            params.param("session", url.param("session"));
            urls.push_back(params);
            for (Url::param_const_iterator i = url.param_begin(); i != url.param_end(); ++i) {
                if (i->key() == "url") {
                    urls.push_back(Url(i->value()));
                } else if (i->key() == "rid") {
                    urls.push_back(Url("rid:///" + i->value()));
                }
            }
            sinks_.push_back(sink);
            if (urls.empty()) {
                sink->set_error(framework::system::logic_error::item_not_exist);
                return;
            }
            GetResourceEvent * event = new GetResourceEvent(sink);
            pending_sinks_.push_back(event);
            event->on(
                boost::bind(&SinkManager::on_event, this, _1, _2));
            rmgr_.async_get(urls, *event);
        }

        void SinkManager::del_sink(
            Sink * sink)
        {
            LOG_INFO("[del_sink] sink:" << sink << ", url:" << sink->url().to_string());
            if (sink->attached()) {
                sink->close();
            } else {
                std::vector<GetResourceEvent *>::iterator iter =
                    std::find_if(pending_sinks_.begin(), pending_sinks_.end(), GetResourceEvent::equal(sink));
                if (iter != pending_sinks_.end())
                    (*iter)->sink = NULL;
            }
            sinks_.erase(std::find(sinks_.begin(), sinks_.end(), sink));
        }

        void SinkManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == rmgr_) {
                GetResourceEvent const & e((GetResourceEvent const &)event);
                std::vector<GetResourceEvent *>::iterator iter = 
                    std::find(pending_sinks_.begin(), pending_sinks_.end(), &e);
                if (iter == pending_sinks_.end())
                    return;
                if (e.sink == NULL) {
                } else if (e.resource) {
                    Resource & r = *e.resource;
                    LOG_DEBUG("[on_event] got resource id=" << r.id());
                    Downloader * downloader = dmgr_.get(r.id());
                    downloader->add_sink(e.sink);
                } else {
                    e.sink->set_error(rmgr_.last_error());
                }
                delete &e;
                pending_sinks_.erase(iter);
            } else if (observable == tmgr_) {
                //LOG_INFO("[on_event] t_100_ms");
            }
        }

    } // namespace client
} // namespace trip
