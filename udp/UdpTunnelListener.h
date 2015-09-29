// UdpTunnelListener.h

#ifndef _TRIP_CLIENT_UDP_UDP_TUNNEL_LISTENER_H_
#define _TRIP_CLIENT_UDP_UDP_TUNNEL_LISTENER_H_

#include "trip/client/udp/UdpSession.h"

namespace trip
{
    namespace client
    {

        class UdpManager;
        struct MessageRequestConnect;

        class UdpTunnelListener
            : public UdpSession
        {
        public:
            UdpTunnelListener(
                UdpManager & manager, 
                UdpTunnel & tunnel);

            virtual ~UdpTunnelListener();

        public:
            virtual void on_recv(
                void * head, 
                NetBuffer & buf);

            virtual void on_msg(
                Message * msg);

        private:
            UdpManager & mgr_;
            boost::uint16_t seq_;
            UdpTunnel * recent_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_TUNNEL_LISTENER_H_
