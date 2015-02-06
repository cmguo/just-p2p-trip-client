// ResourceData.cpp

#include "trip/client/Common.h"
#include "trip/client/core/ResourceData.h"

namespace trip
{
    namespace client
    {

        ResourceData::ResourceData()
            : next_(0)
            , end_(0)
        {
        }

        ResourceData::~ResourceData()
        {
        }

        boost::uint16_t ResourceData::piece_size(
            boost::uint64_t id) const
        {
            return get_segment(id).piece_size(id);
        }

        bool ResourceData::save_segment(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta == NULL || segment.seg == NULL) {
                return false;
            }
            if (segment.seg->cache_md5sum() != segment.meta->md5sum)
                return false;
            if (segment.seg->save(path))
                return false;
            segment.saved = true;
            return true;
        }

        bool ResourceData::load_segment(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.meta == NULL) {
                return false;
            }
            if (Segment::file_md5sum(path) != segment.meta->md5sum)
                return false;
            segment.saved = true;
            return true;
        }

        boost::uint64_t ResourceData::set_piece(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            boost::uint64_t nextid = modify_segment(id).set_piece(id, piece);
            if (nextid == MAKE_ID(0, MAX_BLOCK, MAX_PIECE)) {
                boost::uint64_t index = SEGMENT(id);
                if (next_ == index) {
                    while (true) {
                        ++next_;
                        if (next_ == end_) {
                            next_ = MAX_SEGMENT;
                            return MAKE_ID(next_, 0, nextid);
                        }
                        Segment & segment(modify_segment(MAKE_ID(next_, 0, 0)));
                        if (!segment.finished()) {
                            return MAKE_ID(next_, 0, segment.next());
                        }
                    }
                }
            } else {
                Segment & segment(modify_segment(MAKE_ID(next_, 0, 0)));
                return MAKE_ID(next_, 0, segment.next());
            }
            return 0;
        }

        Block const * ResourceData::map_block(
            boost::uint64_t id, 
            boost::filesystem::path const & path)
        {
            return modify_segment(id).map_block(id, path);
        }

        bool ResourceData::unmap_block(
            boost::uint64_t id)
        {
            return modify_segment(id).unmap_block(id);
        }

        ResourceData::Segment2 const & ResourceData::get_segment2(
            boost::uint64_t id) const
        {
            boost::uint64_t index = SEGMENT(id);
            std::map<boost::uint64_t, Segment2>::const_iterator iter = 
                segments_.find(index);
            if (iter == segments_.end()) {
                static Segment2 empty_segment;
                return empty_segment;
            }
            return iter->second;
        }

        ResourceData::Segment2 & ResourceData::modify_segment2(
            boost::uint64_t id)
        {
            boost::uint64_t index = SEGMENT(id);
            std::map<boost::uint64_t, Segment2>::iterator iter = 
                segments_.find(index);
            if (iter == segments_.end()) {
                iter = segments_.insert(std::make_pair(index, Segment2())).first;
            }
            return iter->second;
        }

        Segment const & ResourceData::get_segment(
            boost::uint64_t id) const
        {
            Segment2 const & segment(get_segment2(id));
            if (segment.seg == NULL) {
                static Segment empty_segment;
                return empty_segment;
            }
            return *segment.seg;
        }

        Segment & ResourceData::modify_segment(
            boost::uint64_t id)
        {
            Segment2 & segment(modify_segment2(id));
            if (segment.seg == NULL) {
                segment.seg = new Segment(segment.meta ? segment.meta->bytesize : 0);
            }
            return *segment.seg;
        }

    } // namespace client
} // namespace trip
