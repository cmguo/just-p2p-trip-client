// Bootstrap.h

#ifndef _TRIP_CLIENT_MAIN_BOOTSTRAP_H_
#define _TRIP_CLIENT_MAIN_BOOTSTRAP_H_

#include <util/protocol/http/HttpClient.h>
#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class Resource;

        class Bootstrap
            : public util::daemon::ModuleBase<Bootstrap>
        {
        public:
            Bootstrap(
                 util::daemon::Daemon & daemon);

            ~Bootstrap();

        public:
            bool get(
                std::string const & name, 
                Url & url);
            
            static bool get(
                boost::asio::io_service & io_svc, 
                std::string const & name, 
                Url & url);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void handle_fetch(
                boost::system::error_code ec);

        private:
            util::protocol::HttpClient http_;
            std::map<std::string, Url> urls_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_BOOTSTRAP_H_
