// CdnManager.h

#ifndef _TRIP_CDN_CDN_MANAGER_H_
#define _TRIP_CDN_CDN_MANAGER_H_

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class TimerManager;
        class Finder;
        class CdnTunnel;

        class CdnManager
            : public util::daemon::ModuleBase<CdnManager>
        {
        public:
            CdnManager(
                util::daemon::Daemon & daemon);

            ~CdnManager();

        public:
            Finder * finder()
            {
                return finder_;
            }

            CdnTunnel & get_tunnel(
                Url const & url);

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
            Finder * finder_;
            std::map<std::string, CdnTunnel *> tunnels_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CDN_CDN_MANAGER_H_
