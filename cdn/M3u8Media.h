// M3u8Media.h

#ifndef _TRIP_CDN_M3U8_MEDIA_H_
#define _TRIP_CDN_M3U8_MEDIA_H_

#include "trip/client/core/Segment.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>

namespace util { namespace stream { 
    class UrlSource;
}}

namespace trip
{
    namespace client
    {

        class M3u8Media
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            M3u8Media(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~M3u8Media();

        public:
            void async_open(
                response_type const & resp);

            void cancel(
                boost::system::error_code & ec);

            void close(
                boost::system::error_code & ec);

        public:
            void on_error(
                boost::system::error_code & ec) const;

        public:
            size_t segment_count() const;

            bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const;

            bool segment_info(
                size_t segment, 
                SegmentMeta & info,
                boost::system::error_code & ec) const;

        private:
            void handle_open(
                boost::system::error_code ec);

            void handle_read(
                boost::system::error_code ec);

            void handle_timer(
                boost::system::error_code ec);

            void parse_m3u8(
                boost::system::error_code & ec);

            void complete_url(
                framework::string::Url & url,
                std::string const & str) const;

        private:
            mutable boost::mutex mutex_;
            bool closed_;

            framework::string::Url url_;
            bool noshift_;

            util::stream::UrlSource * source_;
            boost::asio::deadline_timer timer_;
            boost::asio::streambuf buf_;

            struct StreamInfo;

            bool has_end_;
            size_t seq_start_;
            size_t seq_lastest_;
            mutable size_t seq_play_;
            boost::uint64_t duration_;
            std::vector<std::string> stream_urls_;
            std::vector<SegmentMeta> segments_;
            std::vector<std::string> segment_urls_;
            mutable std::string segment_url_prefix_;
            mutable std::string::size_type segment_url_subpos_;

            response_type resp_;
        };

    } // client
} // trip

#endif // _TRIP_CDN_M3U8_MEDIA_H_
