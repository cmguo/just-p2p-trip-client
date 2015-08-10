// HttpSession.h

#ifndef _TRIP_CLIENT_HTTP_HTTP_SESSION_H_
#define _TRIP_CLIENT_HTTP_HTTP_SESSION_H_

#include "trip/client/core/Sink.h"

#include <util/protocol/http/HttpFieldRange.h>
#include <util/stream/Sink.h>

#include <boost/function.hpp>

namespace trip
{
    namespace client
    {

        class HttpServer;
        class Resource;
        struct ResourceMeta;
        struct ResourceStat;
        struct SegmentMeta;

        using util::protocol::http_field::RangeUnit;

        class HttpSession
            : public Sink
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &)> response_t;

        public:
            HttpSession(
                boost::asio::io_service & io_svc, 
                Resource & resource);

            virtual ~HttpSession();

        public:
            bool empty() const;

        public:
            void async_open(
                HttpServer * server,
                response_t const & resp);

            void async_open(
                HttpServer * server,
                boost::uint64_t segm, 
                response_t const & resp);

            void async_fetch(
                HttpServer * server,
                RangeUnit const & range, 
                util::stream::Sink * sink,
                response_t const & resp);

            bool close_request(
                HttpServer * server, 
                boost::system::error_code & ec);

        private:
            virtual void on_meta(
                ResourceMeta const & meta);

            virtual void on_segment_meta(
                boost::uint64_t segm, 
                SegmentMeta const & meta);

            virtual void on_data();

        private:
            void fetch_next();

            void on_write(
                boost::system::error_code const & ec, 
                size_t bytes_write);

        private:
            boost::asio::io_service & io_svc_;
            struct Request;
            struct find_request;
            std::list<Request> open_requests_;
            std::list<Request> fetch_requests_;
            Piece::pointer piece_;
        };

    } // namespace dispatch
} // namespace trip

#endif // _TRIP_CLIENT_HTTP_SESSION_H_
