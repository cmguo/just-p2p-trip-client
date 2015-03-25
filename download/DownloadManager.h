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
        class Finder;
        class Source;

        class DownloadManager
            : public util::daemon::ModuleBase<DownloadManager>
        {
        public:
            DownloadManager(
                util::daemon::Daemon & daemon);

            ~DownloadManager();

        public:
            void find_sources(
                Downloader & downloader, 
                std::string const & proto, 
                size_t count);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            void on_event(
                util::event::Observable const & observable, 
                util::event::Event const & event);

            void handle_find(
                boost::system::error_code const & ec, 
                Uuid const & rid, 
                std::vector<Source *> const & sources);

        private:
            ResourceManager & rmgr_;
            std::map<Uuid, Downloader *> downloaders_;
            std::map<std::string, Finder *> finders_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_DOWNLOAD_DOWNLOAD_MANAGER_H_
