// DownloadManager.h

#ifndef _TRIP_CLIENT_DOWNLOAD_DOWNLOAD_MANAGER_H_
#define _TRIP_CLIENT_DOWNLOAD_DOWNLOAD_MANAGER_H_

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class Downloader;
        class ResourceManager;

        class DownloadManager
            : public util::daemon::ModuleBase<DownloadManager>
        {
        public:
            DownloadManager(
                util::daemon::Daemon & daemon);

            ~DownloadManager();

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
            ResourceManager & rmgr_;
            std::map<Uuid, Downloader *> downloaders_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_DOWNLOAD_MANAGER_H_
