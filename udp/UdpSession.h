// UdpSession.h

#ifndef _TRIP_CLIENT_UDP_UDP_SESSION_H_
#define _TRIP_CLIENT_UDP_UDP_SESSION_H_

#include "trip/client/proto/Message.h"
#include "trip/client/net/Cell.h"

namespace trip
{
    namespace client
    {

        class UdpTunnel;

        class UdpSession
            : public Cell
        {
        public:
            UdpSession();

            UdpSession(
                UdpTunnel & tunnel);

            virtual ~UdpSession();

        public:
            UdpTunnel & tunnel();

            bool is_open() const;

        public:
            virtual void on_send(
                //void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                //void * head, 
                NetBuffer & buf);

        public:
            virtual void on_tunnel_connecting();

            virtual void on_tunnel_disconnect();

        protected:
            bool send_msg(
                Message * msg);

            void pass_msg_to(
                Message * msg, 
                UdpSession * to);

            virtual void on_msg(
                Message * msg);
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_SESSION_H_
