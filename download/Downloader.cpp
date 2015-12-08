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
            , master_(NULL)
        {
            resource.sink_changed.on(boost::bind(&Downloader::on_event, this, _2));
            resource.seg_meta_ready.on(boost::bind(&Downloader::on_event, this, _2));
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
                LOG_INFO("[on_event] download_point_=" << download_point_);
                switch(e.type){
                case 0: // add.
                    if (master_ == NULL) {
                        master_ = e.sink;
                        play_point_ = download_point_ = master_->position();
                        r.seek(download_point_);
                        reset();
                    }
                    break;
                case 1:  // del.
                    if (master_ != NULL
                        && master_ == e.sink) {
                        if (r.get_sinks().size() > 0) {
                            master_ = r.get_sinks()[0]; 
                            play_point_ = download_point_ = master_->position();
                            r.seek(download_point_);
                            reset();
                        } else { // To stop download.
                            master_ = NULL;
                        }
                    }
                    break;
                case 2: // modify.
                    if (master_ == e.sink) {
                        if (play_point_ != master_->position() || download_point_ != master_->position()) {
                            play_point_ = download_point_ = master_->position();
                            r.seek(download_point_);
                            reset();
                        }
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
            if (master_ 
                && master_->position().segment != play_point_.segment
                && master_->position().top == 0) {
                LOG_INFO("[on_timer] move play point: " << master_->position());
                play_point_ = master_->position();
                reset();
            }
        }

    } // namespace client
} // namespace trip
