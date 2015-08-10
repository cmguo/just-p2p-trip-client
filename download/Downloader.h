// Downloader.h

#ifndef _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_
#define _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_

#include "trip/client/core/Scheduler.h"

#include <util/daemon/Module.h>
#include <util/event/Event.h>

#include <framework/timer/ClockTime.h>

namespace trip
{
    namespace client
    {

        class DownloadManager;
        class Finder;
        class Sink;
        struct SegmentMeta;

        class Downloader
            : public Scheduler
        {
        public:
            Downloader(
                DownloadManager & mgr, 
                Resource & resource);

            ~Downloader();

        public:
            void on_sources(
                Finder & finder, 
                std::vector<Url> const & urls);

            virtual void on_timer(
                Time const & now);

        protected:
            virtual void add_source(
				Source * source) = 0;

			virtual void del_source(
				Source * source) = 0;

            void find_sources(
                std::string const & proto, 
                size_t count);
			
			virtual void reset() = 0;

			virtual void update_segment(
                DataId sid, 
                SegmentMeta const & meta) = 0;

        private:
            friend class DownloadManager;
            void on_event(
                util::event::Event const & event);

        private:
			struct DownloadInfo {
				DataId download_point_;
				Sink const * master_;
                DownloadInfo() : master_(NULL){}
			};
            DownloadManager & mgr_;
            std::vector<Source *> sources_;
        protected:
			DownloadInfo down_info_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_
