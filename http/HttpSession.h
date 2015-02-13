// HttpSession.h

#ifndef _TRIP_CLIENT_HTTP_HTTP_SESSION_H_
#define _TRIP_CLIENT_HTTP_HTTP_SESSION_H_

#include "trip/client/core/Sink.h"

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
            void async_prepare(
                HttpServer * request,
                response_t const & resp);

            void async_fetch(
                HttpServer * request,
                response_t const & resp);

            bool close_request(
                HttpServer * request, 
                boost::system::error_code & ec);

        private:
            void on_event();

        private:
            boost::asio::io_service & io_svc_;
            Resource * resource_;
            std::map<HttpServer *, response_t> requests_;
        };

    } // namespace dispatch
} // namespace trip

#endif // _TRIP_CLIENT_HTTP_SESSION_H_
