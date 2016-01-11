// UdpStun.h

#ifndef _TRIP_CLIENT_UDP_UDP_STUN_H_
#define _TRIP_CLIENT_UDP_UDP_STUN_H_

#include "trip/client/proto/Message.h"
#include "trip/client/net/Cell.h"

namespace trip
{
    namespace client
    {

        class UdpStun
        {
        public:
            UdpStun();

        public:
            virtual void alloc_port(
                );

            virtual void pass_through(
                NetBuffer & buf);

        protected:
            bool send_msg(
                Message * msg);

            void pass_msg_to(
                Message * msg, 
                UdpStun * to);

            virtual void on_msg(
                Message * msg);

        protected:
            boost::uint16_t sid_; // id of remote seesion
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_STUN_H_
