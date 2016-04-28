// MessageTracker.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/udp/UdpEndpoint.h"
#include "trip/client/udp/Serialize.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/Collection.h>
#include <util/serialization/Array.h>

namespace trip
{
    namespace client
    {

        using util::serialization::make_sized;

        /* Login */

        struct MessageRequestLogin
            : MessageData<MessageRequestLogin, REQ_Login>
        {
            boost::uint16_t sid;
            UdpEndpoint endpoint;

            MessageRequestLogin()
                : sid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(sid)
                    & SERIALIZATION_NVP(endpoint);
            }
        };

        struct MessageResponseLogin
            : MessageData<MessageResponseLogin, RSP_Login>
        {
            boost::uint16_t sid;
            boost::uint16_t keepalive;

            MessageResponseLogin()
                : sid(0)
                , keepalive(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(sid);
                    // & SERIALIZATION_NVP(keepalive);
            }
        };

        struct MessageRequestKeepAlive
            : MessageData<MessageRequestKeepAlive, REQ_KeepAlive>
        {
            MessageRequestKeepAlive()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        struct MessageResponseKeepAlive
            : MessageData<MessageResponseKeepAlive, RSP_KeepAlive>
        {
            MessageResponseKeepAlive()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        /* Sync */

        struct MessageRequestSync
            : MessageData<MessageRequestSync, REQ_Sync>
        {
            boost::uint32_t type; // 0 -- full, 1 -- add, 2 -- delete
            std::vector<Uuid> rids;

            MessageRequestSync()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(type)
                    & SERIALIZATION_NVP_NAME("rids", make_sized<boost::uint32_t>(rids));
            }
        };

        struct MessageResponseSync
            : MessageData<MessageResponseSync, RSP_Sync>
        {
            MessageResponseSync()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        /* Port */

        struct MessageRequestPort
            : MessageData<MessageRequestPort, REQ_Port>
        {
            MessageRequestPort()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        struct MessageResponsePort
            : MessageData<MessageResponsePort, RSP_Port>
        {
            Endpoint endpoint;

            MessageResponsePort()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & endpoint;
            }
        };

        /* Logout */

        struct MessageRequestLogout
            : MessageData<MessageRequestLogout, REQ_Logout>
        {
            MessageRequestLogout()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        struct MessageResponseLogout
            : MessageData<MessageResponseLogout, RSP_Logout>
        {
            MessageResponseLogout()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        /* Find */

        struct MessageRequestFind
            : MessageData<MessageRequestFind, REQ_Find>
        {
            Uuid rid;
            boost::uint16_t count;

            MessageRequestFind()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(rid)
                    & SERIALIZATION_NVP(count);
            }
        };

        struct MessageResponseFind
            : MessageData<MessageResponseFind, RSP_Find>
        {
            Uuid rid;
            std::vector<UdpEndpoint> endpoints;

            MessageResponseFind()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(rid)
                    & SERIALIZATION_NVP_NAME("endpoints", make_sized<boost::uint32_t>(endpoints));
            }
        };

        /* Pass */

        struct MessageRequestPass
            : MessageData<MessageRequestPass, REQ_Pass>
        {
            UdpEndpoint endpoint;

            MessageRequestPass()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(endpoint);
            }
        };

        struct MessageResponsePass
            : MessageData<MessageResponsePass, RSP_Pass>
        {
            UdpEndpoint endpoint;

            MessageResponsePass()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(endpoint);
            }
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_
