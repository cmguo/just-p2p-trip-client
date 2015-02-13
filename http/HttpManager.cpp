// HttpManager.cpp

#include "trip/client/Common.h"
#include "trip/client/http/HttpManager.h"
#include "trip/client/http/HttpServer.h"
#include "trip/client/http/HttpSession.h"
#include "trip/client/main/ResourceManager.h"

#include <framework/network/TcpSocket.hpp>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.HttpManager", framework::logger::Debug);

namespace trip
{
    namespace client
    {

        HttpManager::HttpManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<HttpManager>(daemon, "HttpManager")
            , framework::network::ServerManager<HttpServer, HttpManager>(daemon.io_svc())
            , addr_("0.0.0.0:9006+")
        {
            config().register_module("HttpManager")
                << CONFIG_PARAM_NAME_RDWR("addr", addr_);
        }

        HttpManager::~HttpManager()
        {
            assert(closed_sessions_.empty());
            session_map_t::iterator iter = session_map_.begin();
            for (; iter != session_map_.end(); ++iter) {
                delete iter->second;
                iter->second = NULL;
            }
        }

        bool HttpManager::startup(
            boost::system::error_code & ec)
        {
            start(addr_, ec);
            return !ec;
        }

        bool HttpManager::shutdown(
            boost::system::error_code & ec)
        {
            stop();
            return true;
        }

        HttpSession * HttpManager::alloc_session(
            framework::string::Url & url, 
            boost::system::error_code & ec)
        {
            std::string::size_type pos = url.path().find('.');
            if (pos != std::string::npos) {
                url.path(url.path().substr(0, pos));
            }

            std::string session_id = url.param("session");
            bool close = false;

            HttpSession * session = NULL;

            if (!session_id.empty()) {
                if (session_id.compare(0, 5, "close") == 0) {
                    close = true;
                    session_id = session_id.substr(5);
                }
                session_map_t::const_iterator iter = session_map_.find(session_id);
                if (iter != session_map_.end()) {
                    session = iter->second;
                }
            }

            if (session == NULL) {
                ResourceManager & mgr(util::daemon::use_module<ResourceManager>(io_svc()));
                std::vector<Url> urls;
                session = new HttpSession(io_svc(), mgr.get(urls));
                if (session_id.empty()) {
                    session_id = framework::string::format((long)session);
                    url.param("session", session_id);
                }
                session_map_.insert(std::make_pair(session_id, session));
            }

            if (close) {
                session_map_.erase(session_id);
                closed_sessions_.push_back(session);
            }

            return session;
        }

        void HttpManager::free_session(
            HttpSession * session)
        {
            std::vector<HttpSession *>::iterator iter = std::find(
                closed_sessions_.begin(), closed_sessions_.end(), session);
            if (iter != closed_sessions_.end() && (*iter)->empty()) {
                delete *iter;
                closed_sessions_.erase(iter);
            }
        }

    } // namespace client
} // namespace trip