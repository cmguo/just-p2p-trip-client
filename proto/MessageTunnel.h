// MessageTunnel.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_TUNNEL_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_TUNNEL_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/utils/Serialize.h"

namespace trip
{
    namespace client
    {

        struct MessageRequestProbe
            : MessageData<MessageRequestProbe, REQ_Probe>
        {
            boost::uint16_t index;
            boost::uint16_t rand;

            MessageRequestProbe()
                : index(0)
                , rand(::rand())
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & index & rand;
            }
        };

        struct MessageResponseProbe
            : MessageData<MessageResponseProbe, RSP_Probe>
        {
            boost::uint16_t index;
            boost::uint16_t rand;

            MessageResponseProbe()
                : index(0)
                , rand(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & index & rand;
            }
        };

        struct MessageRequestConnect
            : MessageData<MessageRequestConnect, REQ_Connect>
        {
            boost::uint16_t tid;
            Uuid uid;

            MessageRequestConnect()
                : tid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & tid & uid;
            }
        };

        struct MessageResponseConnect
            : MessageData<MessageResponseConnect, RSP_Connect>
        {
            boost::uint16_t tid;

            MessageResponseConnect()
                : tid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & tid;
            }
        };

        struct MessageRequestPing
            : MessageData<MessageRequestPing, REQ_Ping>
        {
            boost::uint32_t timestamp;

            MessageRequestPing()
                : timestamp(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & timestamp;
            }
        };

        struct MessageResponsePing
            : MessageData<MessageResponsePing, RSP_Ping>
        {
            boost::uint32_t timestamp;

            MessageResponsePing()
                : timestamp(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & timestamp;
            }
        };

        struct MessageRequestDisconnect
            : MessageData<MessageRequestDisconnect, REQ_Disconnect>
        {
            MessageRequestDisconnect()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        struct MessageResponseDisconnect
            : MessageData<MessageResponseDisconnect, RSP_Disconnect>
        {
            MessageResponseDisconnect()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_TUNNEL_H_
