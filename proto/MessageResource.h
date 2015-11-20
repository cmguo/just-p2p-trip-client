// MessageIndex.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_RESOURCE_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_RESOURCE_H_

#include "trip/client/core/Resource.h"
#include "trip/client/utils/Serialize.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/vector.h>
#include <util/serialization/Optional.h>

#include <boost/optional.hpp>

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

        struct ResourceBasicInfo
            : ResourceMeta
        {
            boost::uint64_t segcount;

            ResourceBasicInfo()
                : segcount(0)
            {
            }
            
            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                trip::client::serialize(ar, (ResourceMeta &)(*this));
                ar & SERIALIZATION_NVP(segcount);
            }
        };

        struct ResourceInfo
        {
            Uuid id;
            ResourceMeta meta;
            boost::optional<std::vector<Url> > urls;
            boost::optional<std::vector<SegmentMeta> > segments;
            
            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar 
                    & SERIALIZATION_NVP(id)
                    & SERIALIZATION_NVP(meta)
                    & SERIALIZATION_NVP(urls)
                    & SERIALIZATION_NVP(segments);
            }
        };

        template <
            typename Archive
        >
        void serialize(
            Archive & ar, 
            trip::client::ResourceStat & stat)
        {
        }

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_PROTO_MESSAGE_INDEX_H_
