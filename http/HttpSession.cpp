// HttpSession.cpp

#include "trip/client/Common.h"
#include "trip/client/http/HttpSession.h"
#include "trip/client/utils/UrlHelper.h"
#include "trip/client/core/Resource.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/io_service.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.HttpSession", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        using util::protocol::http_field::RangeUnit;

        struct HttpSession::Request
        {
            HttpServer * server;
            boost::uint64_t segm;
            Range range;
            util::stream::Sink * sink;
            HttpSession::response_t resp;

            Request()
                : server(NULL)
                , segm(MAX_SEGMENT)
                , sink(NULL)
            {
            }
        };

        HttpSession::HttpSession(
            boost::asio::io_service & io_svc, 
            Resource & resource)
            : Sink(resource)
            , io_svc_(io_svc)
        {
        }

        HttpSession::~HttpSession()
        {
        }

        bool HttpSession::empty() const
        {
            return open_requests_.empty() && fetch_requests_.empty();
        }

        void HttpSession::async_open(
            HttpServer * server, 
            response_t const & resp)
        {
            LOG_INFO("[async_open] server=" << (void*)server);
            Request r;
            r.server = server;
            r.segm = MAX_SEGMENT;
            if (meta()) {
                io_svc_.post(
                        boost::bind(resp, boost::system::error_code()));
            } else {
                r.resp = resp;
            }
            open_requests_.push_back(r);
        }

        void HttpSession::async_open(
            HttpServer * server, 
            boost::uint64_t segm, 
            Range const & range, 
            response_t const & resp)
        {
            LOG_INFO("[async_open] server=" << (void*)server << ", segment=" << segm << ", range=" << range.to_string());
            Request r;
            r.server = server;
            r.segm = segm;
            r.range = range;
            if (segment_meta(segm)) {
                io_svc_.post(
                        boost::bind(resp, boost::system::error_code()));
            } else {
                r.resp = resp;
            }
            open_requests_.push_back(r);
            if (open_requests_.size() == 1 && fetch_requests_.empty())
                fetch_next();
        }

        struct HttpSession::find_request
        {
            find_request(
                HttpServer * server)
                : server_(server)
            {
            }
            bool operator()(
                Request const & r)
            {
                return r.server == server_;
            }
            HttpServer * server_;
        };

        void HttpSession::async_fetch(
            HttpServer * server, 
            util::stream::Sink * sink,
            response_t const & resp)
        {
            LOG_INFO("[async_fetch] server=" << (void*)server);
            std::list<Request>::iterator iter = 
                std::find_if(open_requests_.begin(), open_requests_.end(), find_request(server));
            assert(iter != open_requests_.end());
            assert(iter->resp.empty());
            iter->sink = sink;
            iter->resp = resp;
            fetch_requests_.push_back(*iter);
            open_requests_.erase(iter);
            if (fetch_requests_.size() == 1) {
                fetch_next();
            }
        }

        bool HttpSession::close_request(
            HttpServer * server, 
            boost::system::error_code & ec)
        {
            LOG_INFO("[close_request] server=" << (void*)server);
            std::list<Request>::iterator iter = 
                std::find_if(open_requests_.begin(), open_requests_.end(), find_request(server));
            if (iter != open_requests_.end()) {
                if (!iter->resp.empty()) {
                    io_svc_.post(
                        boost::bind(iter->resp, boost::asio::error::operation_aborted));
                }
                bool first = (iter == open_requests_.begin());
                open_requests_.erase(iter);
                if (first && fetch_requests_.empty()) fetch_next();
                return true;
            }
            iter = std::find_if(fetch_requests_.begin(), fetch_requests_.end(), find_request(server));
            if (iter != fetch_requests_.end()) {
                bool first = (iter == fetch_requests_.begin());
                if (first && iter->sink)
                    Sink::seek_end(iter->segm);
                if (first && piece_) {
                    iter->sink = NULL;
                } else {
                    if (!iter->resp.empty()) {
                        io_svc_.post(
                            boost::bind(iter->resp, boost::asio::error::operation_aborted));
                    }
                    fetch_requests_.erase(iter);
                    if (first && !fetch_requests_.empty()) fetch_next();
                }
                return true;
            }
            LOG_WARN("[close_request] no such request");
            return false;
        }

        ResourceMeta const * HttpSession::meta()
        {
            return resource().meta();
        }

        SegmentMeta const * HttpSession::segment_meta(
            boost::uint64_t segm)
        {
            return resource().get_segment_meta(DataId(segm, 0, 0));
        }

        void HttpSession::on_meta(
            ResourceMeta const & meta)
        {
            LOG_INFO("[on_meta]");
            boost::system::error_code ec;
            std::list<Request>::iterator iter = open_requests_.begin();
            for (; iter != open_requests_.end(); ++iter) {
                if (iter->segm != MAX_SEGMENT) {
                    continue;
                }
                response_t resp;
                resp.swap(iter->resp);
                resp(ec);
            }
        }

        void HttpSession::on_segment_meta(
            boost::uint64_t segm, 
            SegmentMeta const & meta)
        {
            LOG_INFO("[on_segment_meta] segment=" << segm);
            boost::system::error_code ec;
            std::list<Request>::iterator iter = open_requests_.begin();
            for (; iter != open_requests_.end(); ++iter) {
                if (iter->segm != segm) {
                    continue;
                }
                response_t resp;
                resp.swap(iter->resp);
                resp(ec);
            }
        }

        void HttpSession::fetch_next()
        {
            if (open_requests_.empty() && fetch_requests_.empty())
                return;
            Request & r(fetch_requests_.empty() ? open_requests_.front() : fetch_requests_.front());
            if (r.range.size()) {
                // TODO: support multiple range
                RangeUnit & unit(r.range.front());
                Sink::seek_to(r.segm, 
                    boost::uint32_t(unit.begin()), unit.has_end() ? boost::uint32_t(unit.end()) : 0);
                r.range.pop();
            }
            if (r.sink)
                write(r);
        }

        void HttpSession::on_data()
        {
            //LOG_INFO("[on_data]");
            assert(!piece_);
            Request & r(fetch_requests_.front());
            write(r);
        }

        void HttpSession::on_write(
            boost::system::error_code const & ec, 
            size_t bytes_write)
        {
            //LOG_DEBUG("[on_write] bytes_write=" << bytes_write);
            assert(piece_);
            piece_.reset();
            Request & r(fetch_requests_.front());
            if (r.sink == NULL) {
                response_t resp;
                resp.swap(r.resp);
                fetch_requests_.pop_front();
                resp(boost::asio::error::operation_aborted);
                fetch_next();
                return;
            }
            if (ec) {
                LOG_INFO("[on_write] at end");
                response_t resp;
                resp.swap(r.resp);
                resp(ec);
                return;
            }
            write(r);
        }

        void HttpSession::write(
            Request & r)
        {
            if (at_end()) {
                response_t resp;
                resp.swap(r.resp);
                resp(boost::system::error_code());
                return;
            }
            if ((piece_ = read())) {
                boost::asio::async_write(*r.sink, 
                    boost::asio::buffer(piece_->data(), piece_->size()), 
                    boost::asio::transfer_all(), 
                    boost::bind(&HttpSession::on_write, this, _1, _2));
            }
        }

    } // namespace dispatch
} // namespace trip
