// UdpListener.h

#ifndef _TRIP_CLIENT_UDP_UDP_LISTENER_H_
#define _TRIP_CLIENT_UDP_UDP_LISTENER_H_

#include "trip/client/udp/UdpSession.h"

namespace trip
{
    namespace client
    {

        struct MessageRequestConnect;

        class UdpListener
            : public UdpSession
        {
        public:
            UdpListener(
                UdpTunnel & tunnel);

            virtual ~UdpListener();

        public:
            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_LISTENER_H_
