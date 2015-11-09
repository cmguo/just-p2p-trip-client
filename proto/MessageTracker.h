// MessageTracker.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/udp/UdpEndpoint.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/Collection.h>
#include <util/serialization/Array.h>

namespace trip
{
    namespace client
    {

        using util::serialization::make_sized;
        using framework::container::make_array;

        /* Endpoint */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Endpoint & endpoint)
        {
            ar & SERIALIZATION_NVP_NAME("type", endpoint.type);
            ar & SERIALIZATION_NVP_NAME("endp", endpoint.endp);
        }

        /* UdpEndpoint */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            UdpEndpoint & endpoint)
        {
            ar & SERIALIZATION_NVP_NAME("id", endpoint.id);
            ar & SERIALIZATION_NVP_NAME("endpoints", make_sized<boost::uint32_t>(endpoint.endpoints));
        }

        /* Register */

        struct MessageRequestRegister
            : MessageData<MessageRequestRegister, REQ_Register>
        {
            UdpEndpoint endpoint;
            std::vector<Uuid> rids;

            MessageRequestRegister()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(endpoint)
                    & SERIALIZATION_NVP_NAME("rids", make_sized<boost::uint32_t>(rids));
            }
        };

        struct MessageResponseRegister
            : MessageData<MessageResponseRegister, RSP_Register>
        {
            MessageResponseRegister()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        /* Unregister */

        struct MessageRequestUnregister
            : MessageData<MessageRequestUnregister, REQ_Unregister>
        {
            Uuid pid;
            Uuid rid;

            MessageRequestUnregister()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(pid)
                    & SERIALIZATION_NVP(rid);
            }
        };

        struct MessageResponseUnregister
            : MessageData<MessageResponseUnregister, RSP_Unregister>
        {
            boost::uint32_t timestamp;

            MessageResponseUnregister()
                : timestamp(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(timestamp);
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

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_
