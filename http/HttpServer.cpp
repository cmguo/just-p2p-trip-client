// HttpServer.cpp

#include "trip/client/Common.h"
#include "trip/client/http/HttpServer.h"
#include "trip/client/http/HttpManager.h"
#include "trip/client/http/HttpSession.h"
#include "trip/client/http/M3u8Protocol.h"
#include "trip/client/main/DataGraph.h"
#include "trip/client/proto/MessageResource.h"

#include <util/serialization/ErrorCode.h>
#include <util/archive/XmlOArchive.h>
#include <util/protocol/http/HttpError.h>
using namespace util::protocol;

#include <framework/string/Parse.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/LogAccept.h>

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.HttpServer", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        HttpServer::HttpServer(
            HttpManager & mgr)
            : util::protocol::HttpServer(mgr.io_svc())
            , mgr_(mgr)
            , segment_(MAX_SEGMENT)
            , session_(NULL)
        {
        }

        HttpServer::~HttpServer()
        {
            assert(session_ == NULL);
        }

        void HttpServer::local_process(response_type const & resp)
        {
            std::string peer_addr = framework::network::Endpoint(local_endpoint()).to_string().substr(5);
            url_.from_string("http://" + request_head().host.get_value_or(peer_addr) + request_head().path);

            LOG_DEBUG("[local_process] url:" << url_.to_string());

            LOG_ACCEPT_TRACE
                request_head().get_content(std::cout);

            response_head().connection = http_field::Connection::close;

            boost::system::error_code ec;

            option_ = url_.path();
            std::string::size_type pos = option_.rfind('.');
            if (pos != std::string::npos)
                option_.erase(pos);

            if (option_.compare(0, 6, "/dump/") == 0) {
                option_ = "/dump";
            }

            if (option_.compare(0, 7, "/fetch/") == 0) {
                segment_ = framework::string::parse<boost::uint64_t>(option_.substr(7));
                option_ = "/fetch";
            }

            if (option_ != "/dump"
                && option_ != "/meta"
                && option_ != "/stat"
                && option_ != "/hls"
                && option_ != "/fetch") {
                    ec = http_error::not_found;
            } else {
                Range range(request_head().range.get_value_or(Range()));
                // TODO: support multiple range
                if (range.size() > 1) {
                    ec = http_error::requested_range_not_satisfiable;
                }
            }

            if (!ec && option_ != "/dump")
                session_ = mgr_.alloc_session(url_, ec);

            if (ec) {
                make_error(ec);
                resp(ec, response_data().size());
                return;
            }

            if (option_ == "/dump") {
                std::ostream os(&response_data());
                DataGraph & graph(util::daemon::use_module<DataGraph>(get_io_service()));
                std::string depth_str = url_.param_or("depth", "65536");
                int depth = framework::string::parse<int>(depth_str);
                if (!graph.dump(url_.path().substr(5), os, depth)) {
                    ec = http_error::not_found;
                    make_error(ec);
                }
                resp(ec, response_data().size());
            } else if (option_ == "/fetch") {
                range_ = Range(0);
                if (request_head().range.is_initialized()) {
                    range_ = request_head().range.get();
                }
                session_->async_open(this, segment_, range_, 
                    boost::bind(&HttpServer::handle_open, this, resp, _1));
            } else {
                session_->async_open(this, 
                    boost::bind(&HttpServer::handle_open, this, resp, _1));
            }
        }

        static char const * format_mine[][2] = {
            {"flv", "video/x-flv"}, 
            {"ts", "video/MP2T"}, 
            {"mp4", "video/mp4"}, 
            {"asf", "video/x-ms-asf"}, 
            {"m3u8", "application/x-mpegURL"},
        };

        static char const * content_type(
            std::string const & format)
        {
            for (size_t i = 0; i < sizeof(format_mine) / sizeof(format_mine[0]); ++i) {
                if (format == format_mine[i][0]) {
                    return format_mine[i][1];
                }
            }
            return "video";
        }

        void HttpServer::handle_open(
            response_type const & resp,
            boost::system::error_code ec)
        {
            LOG_TRACE( "[handle_open] session:" << session_ << " ec:" << ec.message());

            if (!ec) {
                if (option_ == "/fetch") {
                    ResourceMeta const * meta = session_->meta();
                    SegmentMeta const * smeta = session_->segment_meta(segment_);
                    // TODO: support multiple range
                    RangeUnit & unit(range_.front());
                    if (unit.has_end() && unit.end() > smeta->bytesize) {
                        ec = http_error::requested_range_not_satisfiable;
                    } else {
                        if (!unit.has_end())
                            unit = RangeUnit(unit.begin(), smeta->bytesize);
                        http_field::ContentRange content_range(smeta->bytesize, unit);
                        if (unit.begin() > 0 || (unit.has_end() && unit.end() < smeta->bytesize)) {
                            response_head().err_code = http_error::partial_content;
                            response_head().content_range = content_range;
                        }
                        response_head()["Content-Type"] = std::string("{") + content_type(meta->file_extension.substr(1)) + "}";
                        if (url_.param("chunked") == "true") {
                            response_head().transfer_encoding = "chunked";
                        }
                        resp(ec, (size_t)content_range.size());
                        return;
                    }
                } else if (option_ == "/meta") {
                    response_head()["Content-Type"] = "{application/xml}";
                    make_meta(ec);
                } else if (option_ == "/stat") {
                    response_head()["Content-Type"] = "{application/xml}";
                    make_stat(ec);
                } else if (option_ == "/hls") {
                    response_head()["Content-Type"] = "{application/x-mpegURL}";
                    make_hls(ec);
                }
            }

            if (ec) {
                make_error(ec);
            }

            resp(ec, response_data().size());
        }

        void HttpServer::transfer_response_data(
            response_type const & resp)
        {
            LOG_ACCEPT_TRACE
                response_head().get_content(std::cout);
            
            if (response_data().size()) {
                util::protocol::HttpServer::transfer_response_data(resp);
            } else {
                assert(option_ == "/fetch");
                boost::system::error_code ec;
                set_non_block(true, ec);
                session_->async_fetch(this, &response_stream(), 
                    boost::bind(&HttpServer::handle_fetch, this, resp, _1));
            }
        }

        void HttpServer::handle_fetch(
            response_type const & resp,
            boost::system::error_code const & ec)
        {
            LOG_TRACE( "[handle_fetch] ec:" << ec.message());
            resp(ec, Size());
        }

        void HttpServer::on_finish()
        {
            LOG_TRACE( "[on_finish] session:"<< session_);
            if (session_) {
                boost::system::error_code ec;
                session_->close_request(this, ec);
                mgr_.free_session(session_);
                session_ = NULL;
            }
        }

        void HttpServer::on_error(
            boost::system::error_code const & ec)
        {
            LOG_INFO("[on_error] session:" << session_ << " ec:" << ec.message());
            if (session_) {
                boost::system::error_code ec;
                session_->close_request(this, ec);
            }
        }

        void HttpServer::make_error(
            boost::system::error_code const & ec)
        {
            response_head().err_code = 500;
            response_head().err_msg = "Internal Server Error";
            response_head()["Content-Type"]="{application/xml}";

            util::archive::XmlOArchive<> oa(response_data());
            oa << ec;
            oa << SERIALIZATION_NVP_NAME("message", ec.message());
        }

        void HttpServer::make_meta(
            boost::system::error_code & ec)
        {
            ResourceMeta const * meta = 
                session_->resource().meta();
            if (meta) {
                ResourceInfo info;
                info.meta = *meta;
                info.urls.reset(std::vector<Url>());
                info.urls.get().resize(2);
                Url & url = info.urls.get().at(0);
                url.from_string("http://" + url_.host_svc() + "/fetch/");
                url.param("session", url_.param("session"));
                Url & url2 = info.urls.get().at(1);
                url2.from_string("http://" + url_.host_svc() + "/null/");
                url2.param("session", "close" + url_.param("session"));
                util::archive::XmlOArchive<> oa(response_data());
                oa << info;
            }
        }

        void HttpServer::make_stat(
            boost::system::error_code& ec)
        {
            ResourceStat stat;
            //session_->resource().stat();
            if (!ec) {
                util::archive::XmlOArchive<> oa(response_data());
                oa << stat;
            }
        }

        void HttpServer::make_hls(
            boost::system::error_code& ec)
        {
            ResourceMeta const * meta = 
                session_->resource().meta();
            M3u8Config conf;
            conf.interval = meta->interval;
            conf.url_format = "http://" + url_.host_svc() 
                + "/fetch/%n" + meta->file_extension 
                + "?session=" + url_.param("session");
            std::vector<SegmentMeta> segments;
            if (session_->resource().segments_ready()) {
                session_->resource().get_segments(segments);
            }
            std::ostream os(&response_data());
            M3u8Protocol::create(os, conf, *meta, segments, ec);
        }

    } // namespace client
} // namespace trip
