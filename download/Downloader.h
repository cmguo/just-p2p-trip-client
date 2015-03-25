// Downloader.h

#ifndef _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_
#define _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_

#include "trip/client/core/Scheduler.h"

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class DownloadManager;

        class Downloader
            : public Scheduler
        {
        public:
            Downloader(
                DownloadManager & mgr, 
                Resource & resource);

            ~Downloader();

        public:
            bool close();

            void add_source(
                Source * source);

            void active_sources(
                std::vector<Source *> const & sources);

        protected:
            void find_sources(
                std::string const & proto, 
                size_t count);

        private:
            friend class DownloadManager;
            void on_event(
                util::event::Event const & event);

        private:
            DownloadManager & mgr_;
            std::vector<Source *> sources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_
