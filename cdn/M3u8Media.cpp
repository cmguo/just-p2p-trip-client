// M3u8Media.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/M3u8Media.h"
#include "trip/client/cdn/Error.h"

#include <util/stream/UrlSource.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Algorithm.h>

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>

#include <sstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.cdn.M3u8Media", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        M3u8Media::M3u8Media(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : closed_(true)
            , url_(url)
            , noshift_(false)
            , source_(NULL)
            , timer_(io_svc)
            , has_end_(true)
            , seq_start_(0)
            , seq_lastest_(0)
            , seq_play_(0)
            , segment_url_subpos_(std::string::npos)
        {
            noshift_ = url_.param("m3u8.noshift") == "true";
        }

        M3u8Media::~M3u8Media()
        {
            if (source_) {
                delete source_;
            }
        }

        void M3u8Media::async_open(
            response_type const & resp)
        {
            closed_ = false;
            boost::system::error_code ec;
            source_ = util::stream::UrlSourceFactory::create(timer_.get_io_service(), url_.protocol(), ec);
            if (source_ == NULL) {
                timer_.get_io_service().post(
                    boost::bind(resp, ec));
                return;
            }
            resp_ = resp;
            source_->async_open(url_, 0, boost::uint64_t(-1), 
                boost::bind(&M3u8Media::handle_open, this, _1));
        }

        void M3u8Media::cancel(
            boost::system::error_code & ec)
        {
            source_->cancel(ec);
            timer_.cancel(ec);
        }

        void M3u8Media::close(
            boost::system::error_code & ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            closed_ = true;

            timer_.cancel(ec);
            source_->cancel(ec);
        }

        size_t M3u8Media::segment_count() const
        {
            boost::mutex::scoped_lock lc(mutex_);

            return !has_end_ ? size_t(-1) : segments_.size();
        }

        bool M3u8Media::segment_url(
            size_t segment, 
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            boost::mutex::scoped_lock lc(mutex_);

            segment += seq_start_;
            if (segment >= seq_lastest_ && segment - seq_lastest_ < segments_.size()) {
                std::string const & str = segment_urls_[segment - seq_lastest_];
                LOG_DEBUG("[segment_url] segment: " << segment << ", seq_lastest: " << seq_lastest_ << ", url: " << str);
                complete_url(url, str);
                seq_play_ = segment;
                ec.clear();
                return true;
            } else if (!has_end_ && segment >= seq_lastest_) {
                ec = boost::asio::error::would_block;
                return false;
            } else {
                ec = framework::system::logic_error::out_of_range;
                return false;
            }
        }

        bool M3u8Media::segment_info(
            size_t segment, 
            SegmentMeta & info,
            boost::system::error_code & ec) const
        {
            boost::mutex::scoped_lock lc(mutex_);

            segment += seq_start_;
            if (segment >= seq_lastest_ && segment - seq_lastest_ < segments_.size()) {
                info = segments_[segment - seq_lastest_];
                ec.clear();
                return true;
            } else if (!has_end_ && segment >= seq_lastest_) {
                ec = boost::asio::error::would_block;
                return false;
            } else {
                ec = framework::system::logic_error::out_of_range;
                return false;
            }
        }

        struct completion_condition
        {
            completion_condition(
                boost::asio::streambuf & buf)
                : buf_(buf)
            {
            }
            boost::asio::streambuf & buf_;
            size_t operator()(
                boost::system::error_code const & error, 
                size_t bytes_transferred)
            {
                if (error) return 0;
                if (bytes_transferred < 7) {
                    return 7 - bytes_transferred;
                } else if (bytes_transferred == 7) {
                    if (strncmp(boost::asio::buffer_cast<char const *>(buf_.data()), "#EXTM3U", 7) != 0)
                        return 0;
                    else
                        return 1024;
                } else {
                    return 1024;
                }
            }
        };

        void M3u8Media::handle_open(
            boost::system::error_code ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            lc.unlock();

            if (ec) {
                handle_read(ec);
                return;
            }
            if (ec) {
                boost::asio::async_read(*source_, buf_, completion_condition(buf_), 
                    boost::bind(&M3u8Media::handle_read, this, _1));
            }
        }

        void M3u8Media::handle_read(
            boost::system::error_code ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            if (ec == boost::asio::error::eof) {
                ec.clear();
            }
            boost::system::error_code ec1;
            source_->close(ec1);
            if (!ec) {
                parse_m3u8(ec);
            }

            if (!ec && segments_.empty()) {
                if (stream_urls_.empty()) {
                    timer_.expires_from_now(
                        boost::posix_time::seconds(1));
                    timer_.async_wait(
                        boost::bind(&M3u8Media::handle_timer, this, _1));
                } else {
                    source_->async_open(url_, 0, boost::uint64_t(-1), 
                        boost::bind(&M3u8Media::handle_open, this, _1));
                }
                return;
            }

            if (!ec && !has_end_) {
                timer_.expires_from_now(
                    boost::posix_time::seconds(duration_));
                timer_.async_wait(
                    boost::bind(&M3u8Media::handle_timer, this, _1));
            }
            
            lc.unlock();

            if (!resp_.empty()) {
                response_type resp;
                resp.swap(resp_);
                resp(ec);
            }
        }

        void M3u8Media::handle_timer(
            boost::system::error_code ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            source_->async_open(url_, 0, boost::uint64_t(-1), 
                boost::bind(&M3u8Media::handle_open, this, _1));
        }

        struct M3u8Media::StreamInfo
        {
        };

        void M3u8Media::parse_m3u8(
            boost::system::error_code & ec)
        {
            char const * const M3U8_BEGIN = "#EXTM3U";
            char const * const M3U8_TARGETDURATION = "#EXT-X-TARGETDURATION";
            char const * const M3U8_SEQUENCE = "#EXT-X-MEDIA-SEQUENCE";
            char const * const M3U8_EXTSTREAMINF = "#EXT-X-STREAM-INF";
            char const * const M3U8_EXTINF = "#EXTINF";
            char const * const M3U8_END  = "#EXT-X-ENDLIST";

            if (buf_.size() < 10) {
                ec = cdn_error::xml_format_error;
                return;
            }

            std::string line;
            std::istream is(&buf_);
            bool begin = false;
            bool end = false;
            bool first = (seq_start_ == 0);
            size_t line_num = 0;
            boost::uint64_t duration = 0;
            size_t sequence = 0;
            size_t sequence_url = 0;
            stream_urls_.clear();
            while (std::getline(is, line)) {
                framework::string::trim(line);
                ++line_num;
                std::istringstream iss(line);
                std::string token;
                std::getline(iss, token, ':');
                if (token == M3U8_BEGIN) {
                    begin = true;
                } else if (!begin) {
                    LOG_WARN("[parse_m3u8] line before begin tag: " << line); 
                } else if (end) {
                    LOG_WARN("[parse_m3u8] line before end tag: " << line); 
                } else if (token == M3U8_END) {
                    end = true;
                } else if (token == M3U8_TARGETDURATION) {
                    iss >> duration_;
                } else if (token == M3U8_SEQUENCE) {
                    if (iss >> sequence) {
                        if (first) {
                            seq_start_ = sequence;
                            seq_lastest_ = sequence;
                        }
                        sequence_url = sequence;
                    }
                } else if (token == M3U8_EXTSTREAMINF) {
                    if (!std::getline(is, line)) {
                        LOG_WARN("[parse_m3u8] no url at line: " << line_num); 
                    }
                    framework::string::trim(line);
                    stream_urls_.push_back(line);
                } else if (token == M3U8_EXTINF) {
                    SegmentMeta info;
                    if (iss >> info.duration)
                        info.duration *= 1000;
                    if (!std::getline(is, line)) {
                        LOG_WARN("[parse_m3u8] no url at line: " << line_num); 
                    }
                    framework::string::trim(line);
                    if (sequence_url < seq_lastest_ + segments_.size()) {
                    } else {
                        segments_.push_back(info);
                        segment_urls_.push_back(line);
                        duration += info.duration;
                        LOG_INFO("[parse_m3u8] new url: (" << sequence_url << ") " << line); 
                    }
                    ++sequence_url;
                }
            }

            if (!stream_urls_.empty()) {
                complete_url(url_, stream_urls_.front());
                segments_.clear();
                segment_urls_.clear();
                return;
            }

            if (segments_.empty()) {
                seq_start_ = 0;
                return;
            }

            if (sequence > seq_play_) {
                sequence = seq_play_;
            }
            if (seq_lastest_ < sequence) {
                segments_.erase(segments_.begin(), segments_.begin() + (sequence - seq_lastest_));
                segment_urls_.erase(segment_urls_.begin(), segment_urls_.begin() + (sequence - seq_lastest_));
                seq_lastest_ = sequence;
            }

            has_end_ = end;
            if (end) {
            } else if (first) {
                boost::uint32_t delay = 0;
                for (size_t i = segments_.size() - 1; i != size_t(-1) && i + 4 > segments_.size(); --i) {
                    delay += (boost::uint32_t)segments_[i].duration;
                }
                if (noshift_) {
                    duration = delay;
                    seq_start_ += segments_.size() > 3 ? segments_.size() - 3 : 0;
                }
            }
        }

        void M3u8Media::complete_url(
            framework::string::Url & url,
            std::string const & str) const
        {
            if (segment_url_subpos_ == std::string::npos) {
                if (str.find("://") == std::string::npos) {
                    if (str[0] == '/') {
                        segment_url_prefix_ = url_.protocol() + "://" + url_.host_svc();
                        segment_url_subpos_ = 0;
                    } else {
                        segment_url_prefix_ = url_.protocol() + "://" + url_.host_svc() + url_.path();
                        std::string::size_type pos = segment_url_prefix_.rfind('/');
                        segment_url_prefix_ = segment_url_prefix_.substr(0, pos + 1);
                        segment_url_subpos_ = 0;
                    }
                } else {
                    segment_url_subpos_ = 0;
                }
            }
            if (segment_url_prefix_.empty()) {
                url.from_string(str);
            } else {
                url.from_string(segment_url_prefix_ + str.substr(segment_url_subpos_));
            }
        }

    } // client
} // trip
