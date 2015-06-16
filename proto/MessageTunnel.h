// MessageTunnel.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_TUNNEL_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_TUNNEL_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/utils/Serialize.h"

namespace trip
{
    namespace client
    {

        struct MessageRequestConnect
            : MessageData<MessageRequestConnect, REQ_Connect>
        {
            Uuid pid;
            boost::uint16_t cid;

            MessageRequestConnect()
                : cid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & pid
                    & cid;
            }
        };

        struct MessageResponseConnect
            : MessageData<MessageResponseConnect, RSP_Connect>
        {
            boost::uint16_t cid;

            MessageResponseConnect()
                : cid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & cid;
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
            : MessageData<MessageRequestDisconnect, RSP_Disconnect>
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
