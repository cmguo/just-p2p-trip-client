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
                std::vector<Source *> const & sources);

            virtual void on_timer(
                Time const & now);

            virtual void on_source_lost(
                Source & source);

        public:
            void add_sink(
                Sink * sink);

            void del_sink(
                Sink * sink);

            void update_sink(
                Sink * sink);

            std::vector<Sink *> & get_sinks()
            {
                return sinks_;
            }

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
            DownloadManager & mgr_;
        protected:
            std::vector<Sink *> sinks_;
            Sink * master_;
            DataId download_point_;
            DataId play_point_;
            std::vector<Source *> sources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_DOWNLOADER_H_
