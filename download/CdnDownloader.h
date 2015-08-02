// CdnDownloader.h

#ifndef _TRIP_CLIENT_DOWNLOAD_CDN_DOWNLOADER_H_
#define _TRIP_CLIENT_DOWNLOAD_CDN_DOWNLOADER_H_

#include "trip/client/download/Downloader.h"

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class DownloadManager;
        class Finder;

        class CdnDownloader
            : public Downloader
        {
        public:
            CdnDownloader(
                DownloadManager & mgr, 
                Resource & resource);

            ~CdnDownloader();

        public:
            bool close();

        protected:
            virtual void add_source(
                Source * source);
			virtual void del_source( 
				Source * source);

			virtual void start();
			
			virtual void reset();
			
			virtual void stop();

            virtual bool get_task(
                Source & source, 
                std::vector<DataId> & pieces);

			virtual void on_timeout(
				DataId const & piece);

        private:
			void prepare_taskwindow(
				size_t seg_count = 4);

			struct SubWindow;

			struct TaskInfo 
			{
				//std::vector<boost::uint64_t> tasks_;
				SubWindow* cur_seg_;
				DataId cur_seg_id_;
			};
			struct SubWindow
			{
				DataId pos_;
				boost::uint32_t count_;
//				std::deque<DataId> timeout_pieces_;
				SubWindow()
					: pos_(0), count_(0) {}
				SubWindow(boost::uint32_t count, DataId pos = DataId(0)) 
					: pos_(pos), count_(count) {}
			};
			struct TaskWindow
			{
				std::deque<SubWindow> sub_;
				DataId beg_id_;
			};

			
			std::vector<Source *> cdn_sources_;
			std::map<Source  *, TaskInfo> task_collection_;

			TaskWindow task_window_;

			std::deque<DataId> timeout_tasks_;
        };


    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_CDN_DOWNLOADER_H_
