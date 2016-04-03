// ResourceEvent.hpp

#ifndef _TRIP_CLIENT_CORE_RESOURCE_EVENT_HPP_
#define _TRIP_CLIENT_CORE_RESOURCE_EVENT_HPP_

#include "trip/client/core/ResourceEvent.h"
#include "trip/client/core/Serialize.h"

#include <util/serialization/ErrorCode.h>

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, resource)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceMetaEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, meta)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceErrorEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, ec)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            ResourceDataEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, id)
                & SERIALIZATION_NVP_1(t, degree)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SegmentMetaEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, id)
                & SERIALIZATION_NVP_1(t, meta)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SourceChangedEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, type)
                & SERIALIZATION_NVP_1(t, source)
                ;
        }

        template <typename Archive>
        void serialize(
            Archive & ar, 
            SinkChangedEvent & t)
        {
            ar & SERIALIZATION_NVP_1(t, type)
                & SERIALIZATION_NVP_1(t, sink)
                ;
        }

    }
}

#endif // _TRIP_CLIENT_MAIN_SERIALIZE_HPP_
