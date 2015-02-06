// UdpMainSession.h

#ifndef _TRIP_CLIENT_UDP_UDP_MAIN_SESSION_H_
#define _TRIP_CLIENT_UDP_UDP_MAIN_SESSION_H_

#include "trip/client/udp/UdpSession.h"

namespace trip
{
    namespace client
    {

        class UdpMainSession
            : public UdpSession
        {
        public:
            UdpMainSession(
                UdpTunnel & tunnel);

            virtual ~UdpMainSession();

        public:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_MAIN_SESSION_H_
