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

        void Downloader::add_sink(
            Sink * sink)
        {
            // handle latest one
            sinks_.push_front(sink);
            master_ = sink;
            master_->attach(*this);
            play_point_ = download_point_ = master_->position();
            resource().seek(download_point_);
            reset();
        }

        void Downloader::del_sink(
            Sink * sink)
        {
            if (master_ == sink) {
                master_->detach();
                master_ = NULL;
            }
            sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sink));
            if (master_ == NULL && sinks_.size() > 0) {
                master_ = sinks_[0]; 
                master_->attach(*this);
                play_point_ = download_point_ = master_->position();
                resource().seek(download_point_);
            }
            reset();
        }

        void Downloader::update_sink(
            Sink * sink)
        {
            if (master_ == sink) {
                if (play_point_ != master_->position() || download_point_ != master_->position()) {
                    play_point_ = download_point_ = master_->position();
                    resource().seek(download_point_);
                    reset();
                }
            }
        }

        void Downloader::on_event(
            util::event::Event const & event)
        {
            if (event == resource().seg_meta_ready) {
                SegmentMetaEvent const & e = (SegmentMetaEvent const &)event;
                update_segment(e.id, *e.meta);
            }
        }

        void Downloader::on_sources(
            std::vector<Source *> const & sources)
        {
            for (size_t i = 0; i < sources.size(); ++i) {
                Source * s = sources[i];
                LOG_DEBUG("[on_sources] new source " << s->url().to_string());
                sources_.push_back(s);
                s->attach(*this);
                add_source(s);
            }
        }

        void Downloader::on_timer(
            framework::timer::Time const & now)
        {
            if (master_ 
                && master_->position().segment != play_point_.segment
                && master_->position().top == 0) {
                LOG_TRACE("[on_timer] move play point: " << master_->position());
                play_point_ = master_->position();
                reset();
            }
        }
 
        void Downloader::on_source_lost(
            Source & source)
        {
            del_source(&source);
            sources_.erase(
                std::remove(sources_.begin(), sources_.end(), &source), sources_.end());
        }

    } // namespace client
} // namespace trip
