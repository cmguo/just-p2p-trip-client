// CdnManager.h

#ifndef _TRIP_CDN_CDN_MANAGER_H_
#define _TRIP_CDN_CDN_MANAGER_H_

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class Finder;

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

        private:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        private:
            Finder * finder_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CDN_CDN_MANAGER_H_
