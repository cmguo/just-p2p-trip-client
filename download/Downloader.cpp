// Downloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Finder.h"

namespace trip
{
    namespace client
    {

        Downloader::Downloader(
            DownloadManager & mgr, 
            Resource & resource)
            : Scheduler(resource)
            , mgr_(mgr)
        {
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

        void Downloader::on_timer(
            framework::timer::Time const & now)
        {
        }

    } // namespace client
} // namespace trip
