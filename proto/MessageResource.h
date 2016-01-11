// MessageIndex.h

#ifndef _TRIP_CLIENT_PROTO_MESSAGE_RESOURCE_H_
#define _TRIP_CLIENT_PROTO_MESSAGE_RESOURCE_H_

#include "trip/client/core/Resource.h"
#include "trip/client/utils/Serialize.h"
#include "trip/client/main/DataGraph.hpp"

#include <util/serialization/NVPair.h>
#include <util/serialization/stl/vector.h>
#include <util/serialization/Optional.h>

#include <framework/string/Digest.hpp>

#include <boost/optional.hpp>

namespace trip
{
    namespace client
    {

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
