// M3u8Protocol.h

#ifndef _TRIP_CLIENT_HTTP_M3U8_PROTOCOL_H_
#define _TRIP_CLIENT_HTTP_M3U8_PROTOCOL_H_

#include <iostream>

namespace trip
{
    namespace client
    {

        struct ResourceMeta;
        struct SegmentMeta;

        struct M3u8Config
        {
            M3u8Config()
                : interval(10000)
                , delay(3000)
                , current(0)
                , shift(30000)
                , live_delay(3000)
                , url_format("/%n.ts")
            {
            }

            boost::uint32_t interval; // 秒
            boost::uint32_t delay; // 退后分段数
            boost::uint32_t current; // 直播当前时间
            boost::uint32_t shift; // 直播时移
            boost::uint32_t live_delay; // 直播退后分段数
            std::string url_format; // %n - 分段号， %t 分段起始时间
        };

        class UrlFormator;

        class M3u8Protocol
        {
        public:
            static bool create(
                std::ostream & out, 
                M3u8Config const & config, 
                ResourceMeta const & meta, 
                std::vector<SegmentMeta> const & segments, 
                boost::system::error_code & ec);

        private:
            static void append_head(
                std::ostream & out, 
                boost::uint32_t interval, 
                boost::uint64_t begin);

            static void append_body(
                std::ostream & out, 
                UrlFormator const & formator, 
                boost::uint32_t interval, 
                boost::uint64_t begin, 
                boost::uint64_t end);

            // 零头
            static void append_remnant(
                std::ostream & out, 
                UrlFormator const & formator, 
                boost::uint32_t interval,
                boost::uint64_t index, 
                boost::uint32_t remnant); // 毫秒

            static void append_body(
                std::ostream & out, 
                UrlFormator const & formator, 
                std::vector<SegmentMeta> const & segments);

            static void append_tail(
                std::ostream & out, 
                bool is_end);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_HTTP_M3U8_PROTOCOL_H_
