// SspManager.h

#ifndef _TRIP_SSP_SSP_MANAGER_H_
#define _TRIP_SSP_SSP_MANAGER_H_

#include "trip/client/ssp/SspEndpoint.h"

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class TimerManager;
        class SspBus;
        class SspTunnel;
        struct SspEndpoint;

        class SspManager
            : public util::daemon::ModuleBase<SspManager>
        {
        public:
            SspManager(
                util::daemon::Daemon & daemon);

            ~SspManager();

        public:
            SspTunnel & get_tunnel(
                SspEndpoint const & ep);

        public:
            SspBus & bus()
            {
                return *bus_;
            }

            SspEndpoint & local_endpoint()
            {
                return local_endpoint_;
            }

            std::map<Uuid, SspTunnel *> const & tunnels() const
            {
                return tunnels_;
            }

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

        private:
            TimerManager & tmgr_;
            SspBus * bus_;
            SspEndpoint local_endpoint_;
            std::map<Uuid, SspTunnel *> tunnels_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_SSP_SSP_MANAGER_H_
