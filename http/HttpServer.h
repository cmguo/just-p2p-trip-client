// HttpServer.h

#ifndef _TRIP_CLIENT_HTTPD_HTTP_SERVER_H_
#define _TRIP_CLIENT_HTTPD_HTTP_SERVER_H_

#include <util/protocol/http/HttpServer.h>
#include <util/protocol/http/HttpRequest.h>
#include <util/protocol/http/HttpResponse.h>

namespace trip
{
    namespace client
    {

        class HttpManager;
        class HttpSession;

        using util::protocol::http_field::Range;
        using util::protocol::http_field::RangeUnit;

        class HttpServer
            : public util::protocol::HttpServer
        {
        public:
            HttpServer(
                HttpManager & mgr);

            ~HttpServer();

        public:
            virtual void transfer_response_data(
                response_type const & resp);

            virtual void on_error(
                boost::system::error_code const & ec);

            virtual void on_finish();

            virtual void local_process(response_type const & resp);

        private:
            void handle_open(
                response_type const & resp,
                boost::system::error_code ec);

            void handle_fetch(
                response_type const & resp,
                boost::system::error_code const & ec);

            void make_error(
                boost::system::error_code const & ec);

            void make_meta(
                boost::system::error_code& ec);

            void make_stat(
                boost::system::error_code& ec);

        private:
            HttpManager & mgr_;
            framework::string::Url url_;
            std::string option_;
            boost::uint64_t segment_;
            HttpSession * session_;
            Range range_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_HTTPD_HTTP_SERVER_H_
