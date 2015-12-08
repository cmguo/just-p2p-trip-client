// CdnManager.cpp

#include "trip/client/Common.h"
#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/CdnFinder.h"
#include "trip/client/cdn/CdnTunnel.h"
#include "trip/client/timer/TimerManager.h"

namespace trip
{
    namespace client
    {

        CdnManager::CdnManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<CdnManager>(daemon, "CdnManager")
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
            , finder_(new CdnFinder(*this))
        {
        }

        CdnManager::~CdnManager()
        {
            std::map<std::string, CdnTunnel *>::iterator iter = tunnels_.begin();
            for (; iter != tunnels_.end(); ++iter)
                delete iter->second;
            delete finder_;
        }

        bool CdnManager::startup(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.on(
                boost::bind(&CdnManager::on_event, this, _1, _2));
            return true;
        }

        bool CdnManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.un(
                boost::bind(&CdnManager::on_event, this, _1, _2));
            return true;
        }

        CdnTunnel & CdnManager::get_tunnel(
            Url const & url)
        {
            std::string host_svc(url.host_svc());
            std::map<std::string, CdnTunnel *>::iterator iter = tunnels_.find(host_svc);
            if (iter == tunnels_.end()) {
                iter = tunnels_.insert(std::make_pair(host_svc, new CdnTunnel(io_svc()))).first;
            }
            return *iter->second;
        }

        void CdnManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == tmgr_) {
                std::map<std::string, CdnTunnel *>::iterator iter = tunnels_.begin();
                for (; iter != tunnels_.end(); ++iter) {
                    iter->second->on_timer(tmgr_.t_100_ms.now);
                }
            }
        }

    } // namespace client
} // namespace trip
