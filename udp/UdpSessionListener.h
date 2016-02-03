// UdpSessionListener.h

#ifndef _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER_H_
#define _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER_H_

#include "trip/client/udp/UdpSession.h"
#include "trip/client/udp/UdpEndpoint.h"

namespace trip
{
    namespace client
    {

        class UdpManager;

        class UdpSessionListener
            : public UdpSession
        {
        public:
            UdpSessionListener(
                UdpManager & manager, 
                UdpTunnel & tunnel);

            virtual ~UdpSessionListener();

        public:
            void set_remote(
                UdpEndpoint const & remote);

            void set_fake_sid(
                boost::uint16_t id);

            UdpEndpoint const & endpoint() const
            {
                return endpoint_;
            }

        public:
            virtual void on_send(
                NetBuffer & buf);

            virtual void on_recv(
                NetBuffer & buf);

            virtual void on_msg(
                Message * msg);

            virtual void on_timer(
                Time const & now);

        private:
            void set_remote(
                boost::uint16_t id);

        protected:
            UdpManager & umgr_;
            UdpEndpoint endpoint_;
            UdpEndpointPairs endpoints_;
            Time msg_time_;
            boost::uint16_t msg_try_;
            boost::uint16_t status_; // 0 - probing, 1 - connecting, 2 - connected
            Endpoint pkt_ep_;
            boost::uint16_t seq_;
            UdpSession * recent_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER_H_
