// SinkManager.h

#ifndef _TRIP_CLIENT_SINK_SINK_MANAGER_H_
#define _TRIP_CLIENT_SINK_SINK_MANAGER_H_

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class Sink;
        class Resource;
        class ResourceManager;
        class TimerManager;
        class DownloadManager;

        class SinkManager
            : public util::daemon::ModuleBase<SinkManager>
        {
        public:
            SinkManager(
                util::daemon::Daemon & daemon);

            ~SinkManager();

        public:
            void add_sink(
                Sink * sink);

            void del_sink(
                Sink * sink);

            std::vector<Sink *> const & sinks() const
            {
                return sinks_;
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
            struct GetResourceEvent;

            ResourceManager & rmgr_;
            TimerManager & tmgr_;
            DownloadManager & dmgr_;
            std::vector<Sink *> sinks_;
            std::vector<GetResourceEvent *> pending_sinks_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_SINK_SINK_MANAGER_H_
