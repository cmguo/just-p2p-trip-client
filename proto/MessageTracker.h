// MessageTracker.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/utils/Serialize.h"

#include <util/serialization/Collection.h>

namespace trip
{
    namespace client
    {

        using util::serialization::make_sized;

        /* Register */

        struct MessageRequestRegister
            : MessageData<MessageRequestRegister, REQ_Register>
        {
            Uuid pid;
            std::vector<Uuid> rids;

            MessageRequestRegister()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & pid
                    & make_sized<boost::uint32_t>(rids);
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
                ar & pid
                    & rid;
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
                ar & timestamp;
            }
        };

        /* Find */

        struct MessageRequestFind
            : MessageData<MessageRequestFind, REQ_Find>
        {
            MessageRequestFind()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        struct MessageResponseFind
            : MessageData<MessageResponseFind, RSP_Find>
        {
            Uuid rid;

            MessageResponseFind()
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

#endif // _TRIP_CLIENT_PROTO_MESSAGE_TRACKER_H_
