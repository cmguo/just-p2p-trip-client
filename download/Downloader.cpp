// Downloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Finder.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

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
			Resource r = resource();

			SinkChangedEvent& e = (SinkChangedEvent&)event;
			switch(e.type){
				case 0: // add.
					if (master_ == NULL) {
						master_ = e.sink;
						down_info_.download_point_ =  e.sink->position();
					}
					break;
				case 1:  // del.
					if (master_ != NULL
						&& master_ == e.sink) {
						if (r.get_sinks().size() > 0)
							master_ = resource().sinks_[0]; 
						else { // To stop download.

						}
					}
					break;
				case 2: // modify.
					if (master_ != NULL
						&& master_ == e.sink) {
						if (down_info_.download_point_ != e.sink->position())
							down_info_.download_point_ = e.sink->position();
						stop();
						reset();
						start();

					}
					break;
				default:
					LOG_ERROR("[on_event] unexpect event:" << e.type);
			}
		}

        void Downloader::active_sources(
            Finder & finder, 
            std::vector<Url> const & urls)
        {
            for (size_t i = 0; i < urls.size(); ++i) {
                Url const & url(urls[i]);
                bool found = false;
                for (size_t j = 0; j < sources_.size(); ++j) {
                    if (url == sources_[j]->url()) {
                        if (!sources_[j]->attached()) {
                            sources_[j]->attach(*this);
                            add_source(sources_[j]);
                            found = true;
                        }
                    }
                }
                if (!found) {
                    Source * s = finder.create(resource(), url);
                    sources_.push_back(s);
                    s->attach(*this);
                    add_source(s);
                }
            }
        }
/*
		void Downloader::add_source( Source * source )
		{
			sources_.push_back(source);
		}


		void Downloader::del_source( Source * source )
		{
			//sources_.remove()
		}
*/

        void Downloader::on_timer(
            framework::timer::Time const & now)
            Time const & now)
        {
        }

    } // namespace client
} // namespace trip
