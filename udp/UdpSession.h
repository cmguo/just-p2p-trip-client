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
            UdpSession(
                UdpTunnel & tunnel);

            virtual ~UdpSession();

        public:
            virtual void on_send(
                void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                void * head, 
                NetBuffer & buf);

        protected:
            bool send_msg(
                Message * msg);

            virtual void on_msg(
                Message * msg) = 0;

        protected:
            boost::uint16_t peer_id_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_SESSION_H_
