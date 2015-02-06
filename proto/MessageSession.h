// MessageSession.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_SESSION_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_SESSION_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/core/Segment.h"
#include "trip/client/core/Piece.h"

#include <util/serialization/Collection.h>

namespace trip
{
    namespace client
    {

        using util::serialization::make_sized;

        /* Bind */

        struct MessageRequestBind
            : MessageData<MessageRequestBind, REQ_Bind>
        {
            Uuid rid;
            boost::uint16_t sid;

            MessageRequestBind()
                : sid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & rid
                    & sid;
            }
        };

        struct MessageResponseBind
            : MessageData<MessageResponseBind, RSP_Bind>
        {
            boost::uint16_t sid;

            MessageResponseBind()
                : sid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & sid;
            }
        };

        /* Meta */

        struct MessageRequestMeta
            : MessageData<MessageRequestMeta, REQ_Meta>
        {
            boost::uint64_t start; // start index
            boost::uint32_t count;

            MessageRequestMeta()
                : start(0)
                , count(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & start
                    & count;
            }
        };

        template <typename Archive>
        void serialize(
            SegmentMeta & meta, 
            Archive & ar)
        {
            ar & meta.duration
                & meta.bytesize
                & meta.md5sum;
        }

        struct MessageResponseMeta
            : MessageData<MessageResponseMeta, RSP_Meta>
        {
            boost::uint64_t start;
            std::vector<SegmentMeta> vec_meta;

            MessageResponseMeta()
                : start(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & start
                    & make_sized<boost::uint32_t>(vec_meta);
            }
        };

        /* Map */

        struct MessageRequestMap
            : MessageData<MessageRequestMap, REQ_Map>
        {
            boost::uint64_t start; // start index
            boost::uint32_t count;

            MessageRequestMap()
                : start(0)
                , count(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & start
                    & count;
            }
        };

        struct MessageResponseMap
            : MessageData<MessageResponseMap, RSP_Map>
        {
            boost::uint64_t start;
            boost::dynamic_bitset<boost::uint32_t> map;

            MessageResponseMap()
                : start(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & start
                    & map;
            }
        };

        /* Data */

        struct MessageRequestData
            : MessageData<MessageRequestData, REQ_Data>
        {
            boost::uint64_t index; // start index

            MessageRequestData()
                : index(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & index;
            }
        };

        struct MessageResponseData
            : MessageData<MessageResponseData, RSP_Data>
        {
            boost::uint64_t index;
            Piece::pointer piece;

            MessageResponseData()
                : index(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & index;
            }
        };

        /* Unbind */

        struct MessageRequestUnbind
            : MessageData<MessageRequestUnbind, REQ_Unbind>
        {
            MessageRequestUnbind()
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
            }
        };

        struct MessageResponseUnbind
            : MessageData<MessageResponseUnbind, RSP_Unbind>
        {
            MessageResponseUnbind()
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

#endif // _TRIP_CLIENT_PROTO_MESSAGE_SESSION_H_
