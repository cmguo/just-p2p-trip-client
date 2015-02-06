// Client.h

#ifndef _TRIP_CLIENT_MAIN_CLIENT_H_
#define _TRIP_CLIENT_MAIN_CLIENT_H_

#include <util/daemon/Daemon.h>

namespace trip
{
    namespace client
    {
    
        util::daemon::Daemon & global_daemon();

    } // namespace trip
} // namespace trip

#endif // _TRIP_CLIENT_MAIN_CLIENT_H_
