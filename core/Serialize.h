// Serialize.h

#ifndef _TRIP_CLIENT_CORE_SERIALIZE_H_
#define _TRIP_CLIENT_CORE_SERIALIZE_H_

#include "trip/client/core/Resource.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Sink.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/vector.h>
#include <util/serialization/Url.h>
#include <util/serialization/Uuid.h>
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

        template <typename Archive>
        void serialize(
            Archive & ar, 
            DataId & t)
        {
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
            ResourceData & t)
        {
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Resource & t)
        {
            ar & SERIALIZATION_NVP_2(t, id)
                & SERIALIZATION_NVP_2(t, meta)
                & SERIALIZATION_NVP_2(t, urls);
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
            ar & SERIALIZATION_NVP_2(t, type)
                & SERIALIZATION_NVP_2(t, position);
        }

    }
}

#endif // _TRIP_CLIENT_CORE_SERIALIZE_H_
