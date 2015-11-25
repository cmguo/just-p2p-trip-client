// Bootstrap.h

#ifndef _TRIP_CLIENT_MAIN_BOOTSTRAP_H_
#define _TRIP_CLIENT_MAIN_BOOTSTRAP_H_

#include <util/protocol/http/HttpClient.h>
#include <util/daemon/Module.h>

#include <util/event/Event.h>
#include <util/event/Observable.h>

namespace trip
{
    namespace client
    {

        class Resource;

        class Bootstrap
            : public util::daemon::ModuleBase<Bootstrap>
            , public util::event::Observable
        {
        public:
            struct Event : util::event::Event {} ready;

        public:
            Bootstrap(
                 util::daemon::Daemon & daemon);

            ~Bootstrap();

        public:
            bool get(
                std::string const & name, 
                Url & url);
            
            bool get(
                std::string const & name, 
                std::vector<Url> & urls);
            
            static Bootstrap & instance(
                boost::asio::io_service & io_svc);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void handle_fetch(
                boost::system::error_code ec);

        private:
            Url url_;
            util::protocol::HttpClient http_;
            std::map<std::string, std::vector<Url> > urls_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_BOOTSTRAP_H_
