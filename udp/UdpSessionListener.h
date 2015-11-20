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
                UdpTunnel & tunnel, 
                UdpEndpoint const & endpoint);

            virtual ~UdpSessionListener();

        public:
            virtual void on_send(
                void * head, 
                NetBuffer & buf);

            virtual void on_recv(
                void * head, 
                NetBuffer & buf);

            virtual void on_msg(
                Message * msg);

            virtual bool empty() const;

            virtual void on_timer(
                Time const & now);

        private:
            void set_remote(
                boost::uint16_t id);

        protected:
            UdpManager & umgr_;
            UdpEndpoint endpoint_;
            Time msg_time_;
            boost::uint16_t msg_try_;
            boost::uint16_t msg_sent_;
            boost::uint16_t msg_recv_;
            boost::uint16_t probe_rand_[4];
            boost::uint16_t status_; // 0 - probing, 1 - connecting, 2 - connected
            framework::network::Endpoint pkt_ep_;
            boost::uint16_t seq_;
            UdpSession * recent_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_UDP_UDP_SESSION_LISTENER_H_
