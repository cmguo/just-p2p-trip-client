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

        struct HttpSession::Request
        {
            HttpServer * server;
            boost::uint64_t segm;
            RangeUnit range;
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
            if (resource().meta()) {
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
            response_t const & resp)
        {
            LOG_INFO("[async_open] server=" << (void*)server << ", segment=" << segm);
            Request r;
            r.server = server;
            r.segm = segm;
            if (resource().get_segment_meta(MAKE_ID(segm, 0, 0))) {
                io_svc_.post(
                        boost::bind(resp, boost::system::error_code()));
            } else {
                r.resp = resp;
            }
            open_requests_.push_back(r);
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
            RangeUnit const & range, 
            util::stream::Sink * sink,
            response_t const & resp)
        {
            LOG_INFO("[async_fetch] server=" << (void*)server << ", range=" << range.to_string());
            std::list<Request>::iterator iter = 
                std::find_if(open_requests_.begin(), open_requests_.end(), find_request(server));
            assert(iter != open_requests_.end());
            assert(iter->resp.empty());
            iter->range = range;
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
                open_requests_.erase(iter);
            }
            iter = std::find_if(fetch_requests_.begin(), fetch_requests_.end(), find_request(server));
            if (iter != fetch_requests_.end()) {
                Sink::seek_end(iter->segm);
                bool first = (iter == fetch_requests_.begin());
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
            }
            return true;
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
            Request const & r(fetch_requests_.front());
            Sink::seek_to(r.segm, 
                boost::uint32_t(r.range.begin()), 
                r.range.has_end() ? boost::uint32_t(r.range.end()) : 0);
            on_data();
        }

        void HttpSession::on_data()
        {
            //LOG_INFO("[on_data]");
            assert(!piece_);
            Request & r(fetch_requests_.front());
            if ((piece_ = read())) {
                boost::asio::async_write(*r.sink, 
                    boost::asio::buffer(piece_->data(), piece_->size()), 
                    boost::asio::transfer_all(), 
                    boost::bind(&HttpSession::on_write, this, _1, _2));
            }
        }

        void HttpSession::on_write(
            boost::system::error_code const & ec, 
            size_t bytes_write)
        {
            //LOG_INFO("[on_write]");
            assert(piece_);
            piece_.reset();
            Request & r(fetch_requests_.front());
            if (ec || at_end()) {
                response_t resp;
                resp.swap(r.resp);
                resp(ec);
                return;
            }
            if (r.sink == NULL) {
                response_t resp;
                resp.swap(r.resp);
                resp(boost::asio::error::operation_aborted);
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
