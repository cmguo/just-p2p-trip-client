// Downloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/Downloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"

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
            std::vector<Source *> const & sources)
        {
        }

    } // namespace client
} // namespace trip
