// HttpServer.cpp

#include "trip/client/Common.h"
#include "trip/client/http/HttpServer.h"
#include "trip/client/http/HttpManager.h"
#include "trip/client/http/HttpSession.h"
#include "trip/client/http/Serialize.h"

#include <util/serialization/ErrorCode.h>
#include <util/archive/XmlOArchive.h>
using namespace util::protocol;

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

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
            , session_(NULL)
        {
        }

        HttpServer::~HttpServer()
        {
            assert(session_ == NULL);
        }

        void HttpServer::local_process(response_type const & resp)
        {
            //LOG_INFO("[local_process]");

            request_head().get_content(std::cout);

            response_head().connection = util::protocol::http_field::Connection::close;

            std::string peer_addr = framework::network::Endpoint(local_endpoint()).to_string().substr(5);
            url_.from_string("http://" + request_head().host.get_value_or(peer_addr) + request_head().path);

            boost::system::error_code ec;
            session_ = mgr_.alloc_session(url_, ec);

            if (!ec) {
                std::string option = url_.path();
                if (option != "/meta"
                    && option != "/stat"
                    && option != "/fetch") {
                        ec = framework::system::logic_error::invalid_argument;
                }
            }

            if (ec) {
                make_error(ec);
                resp(ec, response_data().size());
                return;
            }

            session_->async_prepare(this, 
                boost::bind(&HttpServer::handle_prepare, this, resp, _1));
        }

        void HttpServer::transfer_response_data(
            response_type const & resp)
        {
            response_head().get_content(std::cout);
            
            if (response_data().size()) {
                util::protocol::HttpServer::transfer_response_data(resp);
            } else {
                assert(url_.path() == "/fetch");
                boost::system::error_code ec;
                set_non_block(true, ec);
                session_->async_fetch(this, 
                    boost::bind(&HttpServer::handle_fetch, this, resp, _1));
            }
        }

        void HttpServer::on_finish()
        {
            LOG_INFO( "[on_finish] session:"<< session_);
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

        char const * format_mine[][2] = {
            {"flv", "video/x-flv"}, 
            {"ts", "video/MP2T"}, 
            {"mp4", "video/mp4"}, 
            {"asf", "video/x-ms-asf"}, 
            {"m3u8", "application/x-mpegURL"},
        };

        char const * content_type(
            std::string const & format)
        {
            for (size_t i = 0; i < sizeof(format_mine) / sizeof(format_mine[0]); ++i) {
                if (format == format_mine[i][0]) {
                    return format_mine[i][1];
                }
            }
            return "video";
        }

        void HttpServer::handle_prepare(
            response_type const & resp,
            boost::system::error_code ec)
        {
            LOG_INFO( "[handle_prepare] session:" << session_ << " ec:" << ec.message());

            std::string option = url_.path();

            if (!ec) {
                if (option == "/fetch") {
                }
            }

            if (ec) {
                make_error(ec);
                resp(ec, response_data().size());
                return;
            }

            if (option == "/fetch") {
                ResourceMeta meta;
                SegmentMeta smeta;
                session_->get_meta(meta, ec)
                    && session_->get_segment_meta(smeta, ec);
                response_head()["Content-Type"] = std::string("{") + content_type(meta.file_extension) + "}";
                if (url_.param("chunked") == "true") {
                    response_head().transfer_encoding = "chunked";
                }
                if (smeta.bytesize == 0) {
                    resp(ec, Size());
                } else if (!request_head().range.is_initialized()) {
                    resp(ec, (size_t)smeta.bytesize);
                } else {
                    util::protocol::http_field::Range & range(request_head().range.get());
                    if (range.count() > 1) {
                        resp(util::protocol::http_error::requested_range_not_satisfiable, Size());
                    } else {
                        util::protocol::http_field::ContentRange content_range(smeta.bytesize, range[0]);
                        response_head().err_code = util::protocol::http_error::partial_content;
                        response_head().content_range = content_range;
                        resp(ec, (size_t)content_range.size());
                    }
                }
            } else {
                response_head()["Content-Type"]="{application/xml}";
                if (option == "/meta") {
                    make_meta(ec);
                } else if (option == "/stat") {
                    make_stat(ec);
                }
                if (ec) {
                    make_error(ec);
                }
                resp(ec, response_data().size());
            }
        }

        void HttpServer::handle_fetch(
            response_type const & resp,
            boost::system::error_code const & ec)
        {
            LOG_INFO( "[handle_fetch] ec:"<<ec.message());
            resp(ec, 0);
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
            ResourceMeta meta;
            session_->get_meta(meta, ec);
            if (!ec) {
                util::archive::XmlOArchive<> oa(response_data());
                oa << meta;
            }
        }

        void HttpServer::make_stat(
            boost::system::error_code& ec)
        {
            ResourceStat stat;
            session_->get_stat(stat, ec);
            if (!ec) {
                util::archive::XmlOArchive<> oa(response_data());
                oa << stat;
            }
        }

    } // namespace client
} // namespace trip
