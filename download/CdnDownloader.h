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

        protected:
            virtual void add_source(
                Source * source);
			virtual void del_source( 
				Source * source);

			virtual void reset();
			
			virtual void update_segment(
                DataId sid, 
                SegmentMeta const & meta);

        public:
            virtual bool get_task(
                Source & source, 
                std::vector<DataId> & pieces);

			virtual void on_timeout(
				DataId const & piece);

        private:
			void prepare_taskwindow(
				size_t seg_count = 4);

			struct SegmentInfo;

			struct SourceInfo 
			{
				SegmentInfo* cur_seg;
			};

			struct SegmentInfo
			{
                bool meta_ready;
				DataId pos;
				DataId end;
  				std::deque<DataId> timeout_pieces_;
				SegmentInfo() : meta_ready(false), pos(0), end(0) {}
                bool empty() { return meta_ready && timeout_pieces_.empty() && pos >= end; }
			};
			
			std::map<Source *, SourceInfo> sources_;

            DataId beg_seg_;
            DataId end_seg_;
            std::deque<SegmentInfo *> segments_;
        };


    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_CDN_DOWNLOADER_H_
