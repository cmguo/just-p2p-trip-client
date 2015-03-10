// DownloadManager.cpp

#include "trip/client/Common.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Resource.h"

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        DownloadManager::DownloadManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<DownloadManager>(daemon, "DownloadManager")
            , rmgr_(util::daemon::use_module<ResourceManager>(daemon))
        {
            //config().register_module("DownloadManager")
            //    << CONFIG_PARAM_NAME_NOACC("path", path_);
        }

        DownloadManager::~DownloadManager()
        {
        }

        bool DownloadManager::startup(
            boost::system::error_code & ec)
        {
            rmgr_.resource_added.on(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            rmgr_.resource_deleting.on(
                boost::bind(&DownloadManager::on_event, this, _1, _2));
            return true;
        }

        bool DownloadManager::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

        void DownloadManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == rmgr_) {
                Resource & r = *rmgr_.resource_added.resource;
                if (event == rmgr_.resource_added) {
                    r.sink_changed.on(
                        boost::bind(&DownloadManager::on_event, this, _1, _2));
                } else {
                    std::map<Uuid, Downloader *>::iterator iter = 
                        downloaders_.find(r.id());
                    if (iter == downloaders_.end()) {
                        r.sink_changed.un(
                            boost::bind(&DownloadManager::on_event, this, _1, _2));
                    } else {
                        iter->second->close();
                        downloaders_.erase(iter);
                    }
                }
            } else {
                Resource & r = (Resource &)observable;
                assert(r.sink_changed == event);
                r.sink_changed.un(
                    boost::bind(&DownloadManager::on_event, this, _1, _2));
                Downloader * downloader = new Downloader(*this, r);
                downloader->on_event(event);
                downloaders_[r.id()] = downloader;
            }
        }

    } // namespace client
} // namespace trip
