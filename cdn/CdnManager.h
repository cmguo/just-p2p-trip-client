// CdnManager.h

#ifndef _TRIP_CDN_CDN_MANAGER_H_
#define _TRIP_CDN_CDN_MANAGER_H_

#include <util/daemon/Module.h>
#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class Resource;
        class Source;
        class Finder;
        class CdnSource;
        class CdnFinder;
        class SspManager;

        class CdnManager
            : public util::daemon::ModuleBase<CdnManager>
        {
        public:
            CdnManager(
                util::daemon::Daemon & daemon);

            ~CdnManager();

        public:
            Finder * finder();

        public:
            typedef std::map<Uuid, // rid
                    std::map<Uuid, CdnSource *> > source_map_t;

            source_map_t const & sources() const
            {
                return sources_;
            }

        private:
            friend class CdnFinder;
            friend class CdnSource;

            CdnSource * get_source(
                Resource & resource, 
                Url const & url);

            void del_source(
                CdnSource * source);

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            SspManager & smgr_;
            CdnFinder * finder_;
            source_map_t sources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CDN_CDN_MANAGER_H_
