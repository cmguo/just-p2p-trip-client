// DataGraph.hpp

#ifndef _TRIP_CLIENT_MAIN_DATA_GRAPH_HPP_
#define _TRIP_CLIENT_MAIN_DATA_GRAPH_HPP_

#include "trip/client/main/ResourceManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Resource.h"

#include <util/serialization/stl/vector.h>
#include <util/serialization/stl/map.h>
#include <util/serialization/Url.h>
#include <util/serialization/Uuid.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceMeta & meta)
        {
            ar & SERIALIZATION_NVP_NAME("duration", meta.duration)
                & SERIALIZATION_NVP_NAME("bytesize", meta.bytesize)
                & SERIALIZATION_NVP_NAME("interval", meta.interval)
                & SERIALIZATION_NVP_NAME("bitrate", meta.bitrate)
                & SERIALIZATION_NVP_NAME("segcount", meta.segcount)
                & SERIALIZATION_NVP_NAME("file_extension", meta.file_extension);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SegmentMeta & meta)
        {
            ar & SERIALIZATION_NVP_NAME("duration", meta.duration)
                & SERIALIZATION_NVP_NAME("bytesize", meta.bytesize)
                & SERIALIZATION_NVP_NAME("md5sum", meta.md5sum);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            Resource & t)
        {
            ar & SERIALIZATION_NVP_NAME("id", t.id())
                & SERIALIZATION_NVP_NAME("meta", t.meta())
                & SERIALIZATION_NVP_NAME("urls", t.urls());
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceManager & t)
        {
            ar & t.resources();
        }

    }
}

namespace util
{
    namespace serialization
    {

        template <class Archive>
        struct is_single_unit<Archive, trip::client::ResourceManager>
            : boost::true_type
        {
        };

    }
}

#endif // _TRIP_CLIENT_MAIN_DATA_GRAPH_HPP_
