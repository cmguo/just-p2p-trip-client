// CdnDownloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/CdnDownloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Finder.h"
#include "trip/client/core/Sink.h"

namespace trip
{
    namespace client
    {

        CdnDownloader::CdnDownloader(
            DownloadManager & mgr, 
            Resource & resource)
            : Downloader(mgr, resource)
        {
            reset();
            find_sources("http", 10);
        }

        CdnDownloader::~CdnDownloader()
        {
        }

        void CdnDownloader::add_source( Source * source )
        {
            if (source->url().protocol() == "http")
            {
                if (sources_.find(source) == sources_.end()) {
                    sources_[source] =   SourceInfo();
                }
            }

        }

        void CdnDownloader::del_source( Source * source )
        {
        }

        void CdnDownloader::update_segment(
            DataId sid, 
            SegmentMeta const & meta)
        {
            if (sid >= beg_seg_ && sid < end_seg_) {
                SegmentInfo * seg = segments_[sid.segment - beg_seg_.segment];
                seg->meta_ready = true;
                seg->end.inc_segment(0);
                seg->end.inc_block_piece(resource().get_segment(sid)->piece_count());
            }
        }

        bool CdnDownloader::get_task( Source & source, std::vector<DataId> & pieces )
        {
            SourceInfo &taskinfo = sources_[&source];
            SegmentInfo * seg = taskinfo.cur_seg;
            boost::uint32_t need_count = source.window_left();

            if (seg && seg->empty()) {
                seg = NULL;
            }

            if (seg == NULL) {
                for (size_t i = 0; i < segments_.size(); ++i) {
                    seg = segments_[i];
                    if (seg->empty() || !source.has_segment(seg->pos))
                        seg = NULL;
                    else
                        break;
                }
                if (seg == NULL)
                    return false;
            }

            std::deque<DataId>::iterator beg = seg->timeout_pieces_.begin();
            std::deque<DataId>::iterator end = seg->timeout_pieces_.end();
            if (seg->timeout_pieces_.size() > need_count) {
                end = beg + need_count;
                need_count = 0;
            } else {
                need_count -= seg->timeout_pieces_.size();
            }
            pieces.insert(pieces.end(), beg, end);
            seg->timeout_pieces_.erase(beg, end);

            while (need_count > 0 && seg->pos < seg->end) {
                pieces.push_back(seg->pos);
                seg->pos.inc_piece();
                --need_count;
            }

            return true;
        }

        void CdnDownloader::prepare_taskwindow(size_t seg_count)
        {
            DataId play_point(down_info_.master_->position());
            play_point.inc_segment(0);

            while (beg_seg_ < play_point) {
                delete segments_.front();
                segments_.pop_front();
                beg_seg_.inc_segment();
            }

            DataId segid(beg_seg_);
            segid.inc_segment(segments_.size());
            while (segments_.size() < seg_count) {
                Resource::Segment2 const *seg = resource().prepare_segment(segid);
                if (seg == NULL) {
                    break;
                }
                SegmentInfo * sinfo = new SegmentInfo;
                sinfo->meta_ready = seg->meta != NULL;
                sinfo->pos = segid;
                sinfo->end = segid;
                sinfo->end.inc_block_piece(seg->seg->piece_count());
                if (seg->saved || seg->seg->full()) {
                    sinfo->pos = sinfo->end;
                } else {
                    seg->seg->seek(sinfo->pos);
                }
                segments_.push_back(sinfo);
                segid.inc_segment();
            }
            end_seg_ = segid;
        }

        void CdnDownloader::reset()
        {
            // Reset.
            for (std::map<Source*, SourceInfo>::iterator iter = sources_.begin();
                iter != sources_.end(); ++iter)
            {
                iter->second.cur_seg = NULL;
            }
            segments_.clear();
            beg_seg_ = down_info_.master_->position();
            prepare_taskwindow(4);
        }

        void CdnDownloader::on_timeout(DataId const& piece)
        {
            if (piece >= beg_seg_ && piece < end_seg_) {
                SegmentInfo * seg = segments_[piece.segment - beg_seg_.segment];
                seg->timeout_pieces_.push_back(piece);
            }
        }



    } // namespace client
} // namespace trip
