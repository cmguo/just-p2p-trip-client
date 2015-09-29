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
            UdpTunnel & tunnel();

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

            void pass_msg_to(
                Message * msg, 
                UdpSession * to);

            virtual void on_msg(
                Message * msg);

        protected:
            boost::uint16_t sid_; // id of remote seesion
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_SESSION_H_
