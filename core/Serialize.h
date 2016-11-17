// Serialize.h

#ifndef _TRIP_CLIENT_CORE_SERIALIZE_H_
#define _TRIP_CLIENT_CORE_SERIALIZE_H_

#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Sink.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/vector.h>
#include <util/serialization/boost/intrusive_ptr.h>
#include <util/serialization/Url.h>
#include <util/serialization/Uuid.h>
#include <util/serialization/Ordered.h>
#include <util/serialization/Digest.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceMeta & t)
        {
            ar & SERIALIZATION_NVP_1(t, duration)
                & SERIALIZATION_NVP_1(t, bytesize)
                & SERIALIZATION_NVP_1(t, interval)
                & SERIALIZATION_NVP_1(t, bitrate)
                & SERIALIZATION_NVP_1(t, segcount)
                & SERIALIZATION_NVP_1(t, file_extension);
        }

        SERIALIZATION_SPLIT_FREE(DataId)

        template <typename Archive>
        void save(
            Archive & ar, 
            DataId const & t)
        {
            std::ostringstream oss;
            oss << t;
            ar & oss.str();
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Piece & t)
        {
            ar & SERIALIZATION_NVP_3(t, nref)
		& SERIALIZATION_NVP_3(t, type)
		& SERIALIZATION_NVP_3(t, data)
		& SERIALIZATION_NVP_3(t, size);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Block & t)
        {
            ar & SERIALIZATION_NVP_3(t, left)
                & SERIALIZATION_NVP_3(t, piece_count)
                & SERIALIZATION_NVP_3(t, last_piece_size);
            ar & SERIALIZATION_NVP_NAME("pieces", framework::container::make_array(t.pieces_));
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Segment & t)
        {
            ar & SERIALIZATION_NVP_3(t, left)
                & SERIALIZATION_NVP_3(t, block_count)
                & SERIALIZATION_NVP_3(t, last_block_size);
            ar & SERIALIZATION_NVP_NAME("blocks", framework::container::make_array(t.blocks_));
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SegmentMeta & t)
        {
            ar & SERIALIZATION_NVP_1(t, duration)
                & SERIALIZATION_NVP_1(t, bytesize)
                & SERIALIZATION_NVP_1(t, md5sum);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Segment2 & t)
        {
            ar & SERIALIZATION_NVP_1(t, id)
                & SERIALIZATION_NVP_1(t, saved)
                & SERIALIZATION_NVP_1(t, external)
                & SERIALIZATION_NVP_1(t, meta)
                & SERIALIZATION_NVP_1(t, seg);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceData::Lock & t)
        {
            ar & SERIALIZATION_NVP_1(t, from)
                & SERIALIZATION_NVP_1(t, to);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceData & t)
        {
            ar & SERIALIZATION_NVP_3(t, next)
                & SERIALIZATION_NVP_3(t, end)
                & SERIALIZATION_NVP_3(t, segments)
                & SERIALIZATION_NVP_3(t, locks)
                & SERIALIZATION_NVP_3(t, meta_dirty)
                & SERIALIZATION_NVP_3(t, meta_ready);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Resource & t)
        {
            ar & SERIALIZATION_NVP_2(t, id)
                & SERIALIZATION_NVP_2(t, meta);
            serialize(ar, (ResourceData &)t);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Source & t)
        {
            ar & SERIALIZATION_NVP_2(t, url)
                & SERIALIZATION_NVP_2(t, window_size)
                & SERIALIZATION_NVP_2(t, window_left);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Sink & t)
        {
            ar & SERIALIZATION_NVP_2(t, url)
                & SERIALIZATION_NVP_2(t, resource)
                // & SERIALIZATION_NVP_2(t, type)
                & SERIALIZATION_NVP_2(t, position)
                & SERIALIZATION_NVP_2(t, attached);
        }

    }
}

namespace util
{
    namespace serialization
    {

        template <class Archive>
        struct is_single_unit<Archive, trip::client::DataId>
            : boost::true_type
        {
        };

    }
}

#endif // _TRIP_CLIENT_CORE_SERIALIZE_H_
