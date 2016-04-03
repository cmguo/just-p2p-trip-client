// Serialize.h

#ifndef _TRIP_CLIENT_CDN_SERIALIZE_H_
#define _TRIP_CLIENT_CDN_SERIALIZE_H_

#include "trip/client/cdn/CdnManager.h"
#include "trip/client/cdn/CdnSource.h"
#include "trip/client/udp/Serialize.h"
#include "trip/client/utils/Serialize.h"

#include <util/serialization/stl/map.h>
#include <util/serialization/Url.h>
#include <util/serialization/Uuid.h>

namespace trip
{
    namespace client
    {

        /* CdnSource */

        template <typename Archive>
        void serialize(
            Archive & ar, 
            CdnSource::PieceRange & t)
        {
            ar
                & SERIALIZATION_NVP_1(t, b)
                & SERIALIZATION_NVP_1(t, e)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            CdnSource & t)
        {
            serialize(ar, (Source &)t);
            serialize(ar, (SspSession &)t);
            ar & SERIALIZATION_NVP_3(t, ranges);
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            CdnManager & t)
        {
            ar & SERIALIZATION_NVP_2(t, sources);
        }

    }
}

#endif // _TRIP_CLIENT_CDN_SERIALIZE_H_
