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
            , timeout_seg_(NULL)
            , lock_(NULL)
        {
            //find_sources("http", 10);
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
                http_sources_.push_back(source);
            } else {
                p2p_sources_.push_back(source);
            }
        }

        void CdnDownloader::del_source(
            Source * source)
        {
            std::vector<Source *> & sources = 
                source->url().protocol() == "http" ? http_sources_ : p2p_sources_;
            sources.erase(
                std::remove(sources.begin(), sources.end(), source), sources.end());
            SegmentInfo * sinfo = (SegmentInfo *)source->context();
            if (sinfo)
                --sinfo->nsource;
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
                --sinfo->nsource;
                LOG_DEBUG("[get_task] finish segment " << sinfo->pos.segment);
                sinfo = NULL;
                source.context(sinfo);
            }

            if (timeout_seg_ && timeout_seg_->empty()) {
                timeout_seg_ = NULL;
                for (size_t i = 0; i < segments_.size(); ++i) {
                    SegmentInfo * seg2 = segments_[i];
                    if (seg2->nsource == 0 && !seg2->timeout_pieces_.empty()) {
                        timeout_seg_ = seg2;
                        break;
                    }
                }
            }

            if (timeout_seg_ && source.has_segment(timeout_seg_->pos)) {
                if (sinfo == NULL)
                    source.context(timeout_seg_);
                sinfo = timeout_seg_;
            }

            if (sinfo == NULL) {
                for (size_t i = 0; i < segments_.size(); ++i) {
                    SegmentInfo * seg2 = segments_[i];
                    // TODO: find better select algorithm, consider
                    // seg2->time_expire, seg2->np2p, seg2->nsource
                    // source->is_http
                    if (!seg2->empty() && source.has_segment(seg2->pos)) {
                        sinfo = seg2;
                        break;
                    }
                }
                source.context(sinfo);
                if (sinfo == NULL) { // TODO: No tasks, how to resume?
                    //LOG_DEBUG("[get_task] no task...");
                    return false;
                }
                ++sinfo->nsource;
            }

            std::set<DataId>::iterator beg = sinfo->timeout_pieces_.begin();
            std::set<DataId>::iterator end = sinfo->timeout_pieces_.end();
            if (sinfo->timeout_pieces_.size() > need_count) {
                end = beg;
                std::advance(end, need_count);
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

        void CdnDownloader::prepare_taskwindow(
            DataId pos, 
            size_t seg_count)
        {
            DataId play_pos(pos);
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
                LOG_DEBUG("[prepare_taskwindow] remove segment, info=" << sinfo << ", pos=" << sinfo->pos);
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
                        LOG_DEBUG("[prepare_taskwindow] insert segment, info=" << sinfo << ", pos=" << sinfo->pos);
                    }
                    for (size_t j = 0; j < p2p_sources_.size(); ++j) {
                        if (p2p_sources_[j]->has_segment(sinfo->pos))
                            ++sinfo->np2p;
                    }
                    segments_[i] = sinfo;
                } else {
                    if (sinfo->pos < play_pos)
                        sinfo->pos = play_pos;
                }
                play_pos.inc_segment();
                play_beg = play_pos;
            }

            if (p2p_sources_.size() < 50)
                find_sources("p2p", 10);

            resource().modify_lock(lock_, win_beg_, win_end_);

            for (size_t i = 0; i < sources_.size(); ++i) {
                Source * src = sources_[i];
                if (src->context() == NULL) {
                    std::vector<DataId> requests;
                    if (get_task(*src, requests))
                        src->request(requests);
                }
            }
        }

        void CdnDownloader::reset()
        {
            if (master_)
                prepare_taskwindow(master_->position(), 4);
            else
                prepare_taskwindow(0, 0);
        }

        void CdnDownloader::on_timeout(
            DataId const & piece)
        {
            if (piece < win_beg_ && piece > win_end_) {
                return;
            }
            SegmentInfo * seg = segments_[piece.segment - win_beg_.segment];
            seg->timeout_pieces_.insert(piece);
            if (seg->nsource == 0 && timeout_seg_ == NULL) {
                timeout_seg_ = seg;
            }
        }

    } // namespace client
} // namespace trip
