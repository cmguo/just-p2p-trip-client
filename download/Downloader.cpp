// Downloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Finder.h"
#include "trip/client/core/Sink.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Downloader", framework::logger::Debug);

        Downloader::Downloader(
            DownloadManager & mgr, 
            Resource & resource)
            : Scheduler(resource)
            , mgr_(mgr)
        {
            resource.sink_changed.on(boost::bind(&Downloader::on_event, this, _2));
            resource.seg_meta_ready.on(boost::bind(&Downloader::on_event, this, _2));
            std::vector<Sink *> const & sinks = resource.get_sinks();
            down_info_.master_ = sinks[0];
            down_info_.download_point_ = sinks[0]->position();
            LOG_INFO("select sink " << (void *)down_info_.master_);
        }

        Downloader::~Downloader()
        {
        }

        void Downloader::find_sources(
            std::string const & proto, 
            size_t count)
        {
            mgr_.find_sources(*this, proto, count);
        }

        void Downloader::on_event(
            util::event::Event const & event)
        {
            Resource & r = resource();
            if (event == r.sink_changed) {
                SinkChangedEvent const & e = (SinkChangedEvent const &)event;
                LOG_INFO("[on_event] sink_changed, type=" << e.type);
                switch(e.type){
                case 0: // add.
                    if (down_info_.master_ == NULL) {
                        down_info_.master_ = e.sink;
                        down_info_.download_point_ =  e.sink->position();
                    }
                    break;
                case 1:  // del.
                    if (down_info_.master_ != NULL
                        && down_info_.master_ == e.sink) {
                        if (r.get_sinks().size() > 0)
                            down_info_.master_ = r.get_sinks()[0]; 
                        else { // To stop download.

                        }
                    }
                    break;
                case 2: // modify.
                    if (down_info_.master_ != NULL
                        && down_info_.master_ == e.sink) {
                        if (down_info_.download_point_ != e.sink->position())
                            down_info_.download_point_ = e.sink->position();
                        //stop();
                        reset();
                        //start();

                    }
                    break;
                default:
                    LOG_ERROR("[on_event] unexpect event:" << e.type);
                }
            } else if (event == r.seg_meta_ready) {
                SegmentMetaEvent const & e = (SegmentMetaEvent const &)event;
                update_segment(e.id, *e.meta);
            }
        }

        void Downloader::on_sources(
            Finder & finder, 
            std::vector<Url> const & urls)
        {
            for (size_t i = 0; i < urls.size(); ++i) {
                Url const & url(urls[i]);
                bool found = false;
                for (size_t j = 0; j < sources_.size(); ++j) {
                    if (url == sources_[j]->url()) {
                        LOG_INFO("[on_sources] old source " << url.to_string());
                        if (!sources_[j]->attached()) {
                            sources_[j]->attach(*this);
                            add_source(sources_[j]);
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    LOG_INFO("[active_sources] new source " << url.to_string());
                    Source * s = finder.create(resource(), url);
                    sources_.push_back(s);
                    s->attach(*this);
                    add_source(s);
                }
            }
        }

        void Downloader::on_timer(
            framework::timer::Time const & now)
        {
        }

    } // namespace client
} // namespace trip
