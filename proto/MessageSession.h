// MessageSession.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_SESSION_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_SESSION_H_

#include "trip/client/proto/MessageData.h"
#include "trip/client/proto/Packet.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/core/Segment.h"
#include "trip/client/core/PoolPiece.h"

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
            boost::uint16_t sid;
            Uuid rid;

            MessageRequestBind()
                : sid(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & sid
                    & rid;
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
            Archive & ar, 
            SegmentMeta & meta)
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
            std::vector<boost::int16_t> offsets;

            MessageRequestData()
                : index(0)
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & index
                    & make_sized<boost::uint8_t>(offsets);
            }
        };

        struct MessageResponseData
            : MessageData<MessageResponseData, RSP_Data>
        {
            boost::uint64_t index;
            Piece::pointer piece;

            MessageResponseData()
                : index(0)
                , piece(PoolPiece::alloc())
            {
            }

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & index;
                Packet * pkt = static_cast<Packet *>(ar.rdbuf());
                if (!pkt->swap_body(piece, Archive::is_loading::value))
                    ar.fail();
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
