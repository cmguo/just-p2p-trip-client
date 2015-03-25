// UdpListener.h

#ifndef _TRIP_CLIENT_UDP_UDP_LISTENER_H_
#define _TRIP_CLIENT_UDP_UDP_LISTENER_H_

#include "trip/client/udp/UdpSession.h"

namespace trip
{
    namespace client
    {

        class UdpManager;
        struct MessageRequestConnect;

        class UdpListener
            : public UdpSession
        {
        public:
            UdpListener(
                UdpManager & manager);

            virtual ~UdpListener();

        public:
            virtual void on_msg(
                Message * msg);

        private:
            UdpManager & mgr_;
            UdpTunnel * recent_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_LISTENER_H_
