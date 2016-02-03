// SspManager.cpp

#include "trip/client/Common.h"
#include "trip/client/ssp/SspManager.h"
#include "trip/client/ssp/SspBus.h"
#include "trip/client/ssp/SspTunnel.h"
#include "trip/client/ssp/SspEndpoint.h"
#include "trip/client/timer/TimerManager.h"

namespace trip
{
    namespace client
    {

        SspManager::SspManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<SspManager>(daemon, "trip.client.SspManager")
            , tmgr_(util::daemon::use_module<TimerManager>(daemon))
            , bus_(new SspBus(io_svc()))
        {
        }

        SspManager::~SspManager()
        {
            delete bus_;
        }

        bool SspManager::startup(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.on(
                boost::bind(&SspManager::on_event, this, _1, _2));
            return true;
        }

        bool SspManager::shutdown(
            boost::system::error_code & ec)
        {
            tmgr_.t_100_ms.un(
                boost::bind(&SspManager::on_event, this, _1, _2));
            return true;
        }

        SspTunnel & SspManager::get_tunnel(
            SspEndpoint const & ep)
        {
            std::map<Uuid, SspTunnel *>::iterator iter = tunnels_.find(ep.id);
            if (iter == tunnels_.end()) {
                iter = tunnels_.insert(std::make_pair(ep.id, new SspTunnel(*bus_, ep))).first;
            }
            return *iter->second;
        }

        void SspManager::on_event(
            util::event::Observable const & observable, 
            util::event::Event const & event)
        {
            if (observable == tmgr_) {
                bus_->handle_timer(tmgr_.t_100_ms.now);
            }
        }

    } // namespace client
} // namespace trip
