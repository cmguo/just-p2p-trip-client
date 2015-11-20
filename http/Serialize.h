// Serialize.h

#include "trip/client/core/Resource.h"

#include <util/serialization/Serialization.h>
#include <util/serialization/NVPair.h>

namespace util
{
    namespace serialization
    {

        template <
            typename Archive
        >
        void serialize(
            Archive & ar, 
            trip::client::ResourceMeta & meta)
        {
            ar & SERIALIZATION_NVP_1(meta, duration);
            ar & SERIALIZATION_NVP_1(meta, interval);
            ar & SERIALIZATION_NVP_1(meta, interval);
            ar & SERIALIZATION_NVP_1(meta, bitrate);
            ar & SERIALIZATION_NVP_1(meta, file_extension);
        }

        template <
            typename Archive
        >
        void serialize(
            Archive & ar, 
            trip::client::ResourceStat & stat)
        {
        }

    }
}
