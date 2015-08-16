// DownloadManager.cpp

#include "trip/client/Common.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/download/CdnDownloader.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/timer/TimerManager.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/p2p/P2pManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Finder.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.DownloadManager", framework::logger::Debug);

        DownloadManager::DownloadManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<DownloadManager>(daemon, "DownloadManager")
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
        {
            //config().register_module("DownloadManager")
            //    << CONFIG_PARAM_NAME_NOACC("path", path_);
            Finder * f = NULL;
            f = util::daemon::use_module<CdnManager>(daemon).finder();
            finders_[f->proto()] = f;
            f = util::daemon::use_module<P2pManager>(daemon).finder();
            finders_[f->proto()] = f;
        }

        DownloadManager::~DownloadManager()
        {
            std::map<std::string, Finder *>::const_iterator iter = finders_.begin();
            for (; iter != finders_.end(); ++iter)
                delete iter->second;
        }

        bool DownloadManager::startup(
            boost::system::error_code & ec)
        {
            rmgr_.resource_added.on(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.on(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            tmgr_.t_100_ms.on(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            return true;
        }

        bool DownloadManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.un(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.un(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            rmgr_.resource_added.un(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            return true;
        }

        void DownloadManager::find_sources(
            Downloader & downloader, 
            std::string const & proto, 
            size_t count)
        {
            std::map<std::string, Finder *>::const_iterator iter = finders_.find(proto);
            if (iter == finders_.end())
                return;
            iter->second->find(downloader.resource(), count, boost::bind(
                    &DownloadManager::handle_find, this, _1, downloader.resource().id(), boost::ref(*iter->second), _2));
        }

        void DownloadManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == rmgr_) {
                Resource & r = *rmgr_.resource_added.resource;
                if (event == rmgr_.resource_added) {
                    LOG_INFO("[on_event] resource_added, id=" << r.id());
                    if (r.get_sinks().empty()) {
                        r.sink_changed.on(
                            boost::bind(&DownloadManager::on_event, this, _1, _2));
                    } else {
                        on_event(r, r.sink_changed);
                    }
                } else {
                    LOG_INFO("[on_event] resource_deleting, id=" << r.id());
                    std::map<Uuid, Downloader *>::iterator iter = 
                        downloaders_.find(r.id());
                    if (iter == downloaders_.end()) {
                        r.sink_changed.un(
                            boost::bind(&DownloadManager::on_event, this, _1, _2));
                    } else {
                        delete iter->second;
                        downloaders_.erase(iter);
                    }
                }
            } else if (observable == tmgr_) {
                //LOG_INFO("[on_event] t_100_ms");
                std::map<Uuid, Downloader *>::iterator iter = 
                    downloaders_.begin();
                for (; iter != downloaders_.end(); ++iter) {
                    iter->second->on_timer(tmgr_.t_100_ms.now);
                }
            } else {
                Resource & r = (Resource &)observable;
                assert(r.sink_changed == event);
                LOG_INFO("[on_event] sink_changed");
                r.sink_changed.un(
                    boost::bind(&DownloadManager::on_event, this, _1, _2));
                Downloader * downloader = NULL;//new Downloader(*this, r);
                downloader = new CdnDownloader(*this, r);
                downloaders_[r.id()] = downloader;
                downloader->on_event(event);
            }
        }

        void DownloadManager::handle_find(
            boost::system::error_code const & ec, 
            Uuid const & rid, 
            Finder & finder, 
            std::vector<Url> const & urls)
        {
            std::map<Uuid, Downloader *>::iterator iter = downloaders_.begin();
            if (iter == downloaders_.end())
                return;
            iter->second->on_sources(finder, urls);
        }

    } // namespace client
} // namespace trip
