// CdnDownloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/CdnDownloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Finder.h"


namespace trip
{
    namespace client
    {
		

        CdnDownloader::CdnDownloader(
            DownloadManager & mgr, 
            Resource & resource)
            : Downloader(mgr, resource)
        {
        }

        CdnDownloader::~CdnDownloader()
        {
        }

		void CdnDownloader::add_source( Source * source )
		{
			if (source->get_protocol() == "http")
			{
				cdn_sources_.push_back(source);
				if (task_collection_.find(source) == task_collection_.end()) {
					task_collection_[source] =   TaskInfo();
				}
			}
			
		}

		void CdnDownloader::del_source( Source * source )
		{
			cdn_sources_.erase(std::remove(cdn_sources_.begin(), cdn_sources_.end(), source));
			//delete source;
		}

		bool CdnDownloader::get_task( Source & source, std::vector<DataId> & pieces )
		{
			boost::uint32_t need_count = source.get_window_left();

			if (!timeout_tasks_.empty())
			{
				for (std::deque<DataId>::iterator iter = timeout_tasks_.begin();
					iter != timeout_tasks_.end();)
				{
					if (source.has_segment(*iter))
					{
						pieces.push_back(*iter);
						timeout_tasks_.erase(iter++);
						--need_count;
						if (need_count <= 0)
							return true;
					}
					else
						iter++;
				}
			}

			TaskInfo &taskinfo = task_collection_[&source];
			while (need_count > 0)
			{
				while (taskinfo.cur_seg_ == NULL)
				{
					if (taskinfo.cur_seg_id_ < task_window_.beg_id_)
						taskinfo.cur_seg_id_ = task_window_.beg_id_;
					else if (taskinfo.cur_seg_id_.top_segment + 1 >= task_window_.beg_id_.top_segment + task_window_.sub_.size())
						return false;
					else
						taskinfo.cur_seg_id_.inc_segment();
					SubWindow &sub = task_window_.sub_[taskinfo.cur_seg_id_.top_segment - task_window_.beg_id_.top_segment];
					if (sub.pos_ < sub.count_ && source.has_segment(taskinfo.cur_seg_id_)) 
						taskinfo.cur_seg_ = &sub;
				}

				if (taskinfo.cur_seg_->pos_ >= taskinfo.cur_seg_->count_) 
					taskinfo.cur_seg_ = NULL;
				else
				{
					pieces.push_back(DataId(taskinfo.cur_seg_id_, 0, taskinfo.cur_seg_->pos_));
					++taskinfo.cur_seg_->pos_;
					--need_count;
				}
			}
			
			return true;
		}

		void CdnDownloader::prepare_taskwindow(size_t seg_count)
		{
			DataId play_point(down_info_.master_->position());

			while (task_window_.beg_id_ < play_point) 
			{
				task_window_.sub_.pop_front();
				task_window_.beg_id_.inc_segment();
			}

			while (task_window_.sub_.size() < seg_count) 
			{

				Segment2 *seg = resource_.prepare_segment(DataId(play_point.top_segment + task_window_.sub_.size(), 0, 0));
				if (seg == NULL)
					break;

				if (seg->saved 
					|| seg->seg->full())
				{
					task_window_.sub_.push_back(SubWindow(0));
				}
				else
				{
					assert(seg->seg != NULL);
					DataId pos(0);
					seg->seg->seek(pos);
					task_window_.sub_.push_back(SubWindow(seg->seg->size() / PIECE_SIZE, pos));
				}

			}
		}

		void CdnDownloader::start()
		{
			
		}

		void CdnDownloader::reset()
		{

			task_window_.sub_.clear();
			task_window_.beg_id_ = down_info_.master_->position();
		}

		void CdnDownloader::stop()
		{
			// Reset.
			for (std::map<Source*, TaskInfo>::iterator iter = task_collection_.begin();
				iter != task_collection_.end(); ++iter)
			{
				iter->second.cur_seg_ = NULL;
			}
			

		}

		void CdnDownloader::on_timeout( std::vector<DataId>& pieces )
		{
			timeout_tasks_.insert(timeout_tasks_.end(), pieces.begin(), pieces.end());
		}



    } // namespace client
} // namespace trip
