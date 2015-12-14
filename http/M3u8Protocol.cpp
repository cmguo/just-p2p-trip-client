// M3u8Protocol.cpp

#include "trip/client/Common.h"
#include "trip/client/http/M3u8Protocol.h"
#include "trip/client/http/UrlFormator.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Segment.h"

namespace trip
{
    namespace client
    {

        /*
            M3U8 协议注意点
            1、EXT-X-MEDIA-SEQUENCE 从 1 开始
            2、最后的零头要按照时间值填写 EXTINF 时长，向上取整
            2、时间值是整数，不能是浮点数，即使最后的零头也必须取整
        */

        bool M3u8Protocol::create(
            std::ostream & out, 
            M3u8Config const & config, 
            ResourceMeta const & meta, 
            std::vector<SegmentMeta> const & segments, 
            boost::system::error_code & ec)
        {
            boost::uint64_t index_beg = 0;
            boost::uint64_t index_end = meta.segcount;
            if (index_end== 0) {
                // live
            }
            append_head(out, config.interval / 1000, index_beg);
            UrlFormator formator(config.url_format);
            if (segments.empty()) {
                append_body(out, formator, config.interval / 1000, index_beg, index_end);
            } else {
                append_body(out, formator, segments);
            }
            append_tail(out, meta.duration != 0);
            ec.clear();
            return true;
        }

        char const * const M3U8_BEGIN = "#EXTM3U";
        char const * const M3U8_TARGETDURATION = "#EXT-X-TARGETDURATION:";
        char const * const M3U8_SEQUENCE = "#EXT-X-MEDIA-SEQUENCE:";
        char const * const M3U8_EXTINF = "#EXTINF:";
        char const * const M3U8_END  = "#EXT-X-ENDLIST";

        void M3u8Protocol::append_head(
            std::ostream & out, 
            boost::uint32_t interval, 
            boost::uint64_t begin)
        {
            out << M3U8_BEGIN << "\n";
            out << M3U8_TARGETDURATION << interval << "\n";
            out << M3U8_SEQUENCE << begin + 1 << "\n";
        }

        void M3u8Protocol::append_body(
            std::ostream & out, 
            UrlFormator const & formator, 
            boost::uint32_t interval, 
            boost::uint64_t begin, 
            boost::uint64_t end)
        {
            boost::uint64_t values[3] = {begin, begin * interval, interval * 1000};
            for (values[0] = begin; values[0] < end; ++values[0], values[1] += interval) {
                out << M3U8_EXTINF << interval << ",\n";
                out << formator(values) << "\n";
            }
        }

        void M3u8Protocol::append_body(
            std::ostream & out, 
            UrlFormator const & formator, 
            std::vector<SegmentMeta> const & segments)
        {
            boost::uint64_t values[4] = {0, 0, 0, 0};
            for (size_t i = 0; i < segments.size(); ++i) {
                values[3] += segments[i].duration;
                values[2] = (values[3] + 999) / 1000 - values[1];
                if (values[2] == 0) values[2] = 1;
                out << M3U8_EXTINF << values[2] << ",\n";
                out << formator(values) << "\n";
                ++values[0];
                values[1] += values[2];
            }
        }

        void M3u8Protocol::append_remnant(
            std::ostream & out, 
            UrlFormator const & formator, 
            boost::uint32_t interval,
            boost::uint64_t index, 
            boost::uint32_t remnant)
        {
            boost::uint64_t values[3] = {index, index * interval, remnant};
            out << M3U8_EXTINF << (remnant + 999) / 1000 << ",\n";
            out << formator(values) << "\n";
        }

        void M3u8Protocol::append_tail(
            std::ostream & out, 
            bool is_end)
        {
            if (is_end) {
                out << M3U8_END << "\n";
            }
        }

    } // namespace client
} // namespace trip
