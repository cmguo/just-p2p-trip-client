// P2pManager.h

#ifndef _TRIP_P2P_P2P_MANAGER_H_
#define _TRIP_P2P_P2P_MANAGER_H_

#include <util/daemon/Module.h>

namespace trip
{
    namespace client
    {

        class Finder;

        class P2pManager
            : public util::daemon::ModuleBase<P2pManager>
        {
        public:
            P2pManager(
                util::daemon::Daemon & daemon);

            ~P2pManager();

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

#endif // _TRIP_P2P_P2P_MANAGER_H_
