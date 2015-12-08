// CdnDownloader.cpp

#include "trip/client/Common.h"
#include "trip/client/download/CdnDownloader.h"
#include "trip/client/download/DownloadManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Finder.h"
#include "trip/client/core/Sink.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.CdnDownloader", framework::logger::Debug);

        CdnDownloader::CdnDownloader(
            DownloadManager & mgr, 
            Resource & resource)
            : Downloader(mgr, resource)
            , lock_(NULL)
        {
            find_sources("http", 10);
            //find_sources("p2p", 10);
            lock_ = resource.alloc_lock(win_beg_, win_end_);
            full_seg_ = new SegmentInfo();
            full_seg_->meta_ready = true;
        }

        CdnDownloader::~CdnDownloader()
        {
            resource().release_lock(lock_);
            delete full_seg_;
        }

        void CdnDownloader::add_source(
            Source * source)
        {
            if (source->url().protocol() == "http") {
                sources_.push_back(source);
            }
        }

        void CdnDownloader::del_source(
            Source * source)
        {
        }

        void CdnDownloader::update_segment(
            DataId sid, 
            SegmentMeta const & meta)
        {
            if (sid >= win_beg_ && sid < win_end_) {
                SegmentInfo * seg = segments_[sid.segment - win_beg_.segment];
                seg->meta_ready = true;
                seg->end.inc_segment(0);
                seg->end.inc_block_piece(resource().get_segment(sid)->piece_count());
            }
        }

        bool CdnDownloader::get_task(
            Source & source, 
            std::vector<DataId> & pieces)
        {
            SegmentInfo * sinfo = (SegmentInfo *)source.context();
            boost::uint32_t need_count = source.window_left();
            
            if (need_count == 0)
                return false;

            if (sinfo && sinfo->empty()) {
                sinfo = NULL;
                source.context(sinfo);
            }

            if (sinfo == NULL) {
                for (size_t i = 0; i < segments_.size(); ++i) {
                    SegmentInfo * seg2 = segments_[i];
                    if (!seg2->empty() && source.has_segment(seg2->pos)) {
                        sinfo = seg2;
                        break;
                    }
                }
                if (sinfo == NULL) // TODO: No tasks, how to resume?
                    return false;
            }

            source.context(sinfo);

            std::deque<DataId>::iterator beg = sinfo->timeout_pieces_.begin();
            std::deque<DataId>::iterator end = sinfo->timeout_pieces_.end();
            if (sinfo->timeout_pieces_.size() > need_count) {
                end = beg + need_count;
                need_count = 0;
            } else {
                need_count -= sinfo->timeout_pieces_.size();
            }
            pieces.insert(pieces.end(), beg, end);
            sinfo->timeout_pieces_.erase(beg, end);

            while (need_count > 0 && sinfo->pos < sinfo->end) {
                pieces.push_back(sinfo->pos);
                sinfo->pos.inc_piece();
                --need_count;
            }

            assert(!pieces.empty());
            return true;
        }

        void CdnDownloader::prepare_taskwindow(size_t seg_count)
        {
            DataId play_pos(master_->position());
            DataId play_beg = play_pos;
            play_beg.inc_segment(0);
            DataId play_end = play_pos;
            play_end.inc_segment(seg_count);

            std::deque<SegmentInfo *> remove;
            if (win_beg_ >= play_end || win_end_ <= play_beg) {
                remove.swap(segments_);
                win_beg_ = win_end_ = play_beg;
            } else if (win_beg_ < play_beg) {
                std::deque<SegmentInfo *>::iterator beg = segments_.begin();
                std::deque<SegmentInfo *>::iterator pos = beg + size_t(play_beg.segment - win_beg_.segment);
                remove.insert(remove.end(), beg, pos);
                segments_.erase(beg, pos);
                win_beg_ = play_beg;
            } else if (play_end < win_end_) {
                std::deque<SegmentInfo *>::iterator beg = segments_.begin();
                std::deque<SegmentInfo *>::iterator pos = beg + size_t(play_end.segment - win_beg_.segment);
                std::deque<SegmentInfo *>::iterator end = segments_.end();
                remove.insert(remove.end(), pos, end);
                segments_.erase(pos, end);
                win_end_ = play_end;
            }

            for (size_t i = 0; i < remove.size(); ++i) {
                SegmentInfo * sinfo = remove[i];
                for (size_t i = 0; i < sources_.size(); ++i) {
                    Source * src = sources_[i];
                    if (src->context() == sinfo) {
                        src->cancel();
                        src->context(full_seg_);
                    }
                }
                delete sinfo;
            }
            remove.clear();

            if (play_beg < win_beg_) {
                segments_.insert(segments_.begin(), win_beg_.segment - play_beg.segment, NULL);
                win_beg_ = play_beg;
            }
            if (win_end_ < play_end) {
                segments_.insert(segments_.end(), play_end.segment - win_end_.segment, NULL);
                win_end_ = play_end;
            }

            resource().modify_lock(lock_, win_beg_, win_end_);

            for (size_t i = 0; i < seg_count; ++i) {
                SegmentInfo * sinfo = segments_[i];
                if (sinfo == NULL) {
                    Segment2 const * seg = resource().prepare_segment(play_pos);
                    if (seg == NULL) {
                        win_end_ = play_beg;
                        segments_.erase(segments_.begin() + i, segments_.end());
                        break;
                    }
                    SegmentInfo * sinfo = new SegmentInfo;
                    sinfo->meta_ready = seg->meta != NULL;
                    sinfo->pos = play_pos;
                    sinfo->end = play_beg;
                    sinfo->end.inc_block_piece(seg->seg->piece_count());
                    if (seg->saved || seg->seg->full()) {
                        sinfo->pos = sinfo->end;
                    } else {
                        seg->seg->seek(sinfo->pos);
                        LOG_INFO("[prepare_taskwindow] segment=" << seg->seg << ", pos=" << sinfo->pos);
                    }
                    segments_[i] = sinfo;
                } else {
                    if (sinfo->pos < play_pos)
                        sinfo->pos = play_pos;
                }
                play_pos.inc_segment();
                play_beg = play_pos;
            }

            resource().modify_lock(lock_, win_beg_, win_end_);

            for (size_t i = 0; i < sources_.size(); ++i) {
                Source * src = sources_[i];
                if (src->context() == NULL) {
                    std::vector<DataId> requests;
                    get_task(*src, requests);
                    src->request(requests);
                }
            }
        }

        void CdnDownloader::reset()
        {
            // TODO: keep old segments_ that can be reused
            prepare_taskwindow(4);
        }

        void CdnDownloader::on_timeout(DataId const& piece)
        {
            if (piece >= win_beg_ && piece < win_end_) {
                SegmentInfo * seg = segments_[piece.segment - win_beg_.segment];
                seg->timeout_pieces_.push_back(piece);
            }
        }



    } // namespace client
} // namespace trip
