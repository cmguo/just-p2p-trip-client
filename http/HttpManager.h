// HttpManager.h

#ifndef _TRIP_CLIENT_HTTP_HTTP_MANAGER_H_
#define _TRIP_CLIENT_HTTP_HTTP_MANAGER_H_

#include <util/daemon/Module.h>
#include <util/event/Event.h>

#include <framework/network/ServerManager.h>

namespace trip
{
    namespace client
    {

        class HttpServer;
        class HttpSession;

        class TimerManager;

        class HttpManager
            : public util::daemon::ModuleBase<HttpManager>
            , public framework::network::ServerManager<HttpServer, HttpManager>
        {
        public:
            HttpManager(
                util::daemon::Daemon & daemon);

            ~HttpManager();

        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        public:
            // avoid ambiguous
            using util::daemon::ModuleBase<HttpManager>::io_svc;

        public:
            HttpSession * alloc_session(
                framework::string::Url & url, 
                boost::system::error_code & ec);

            void free_session(
                HttpSession * session);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

        private:
            framework::network::NetName addr_;
            Duration keep_alive_;

        private:
            TimerManager & tmgr_;
            typedef std::map<std::string, HttpSession *> session_map_t;
            HttpSession * last_session_;
            session_map_t session_map_;
            std::vector<HttpSession *> closed_sessions_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_HTTP_HTTP_MANAGER_H_
