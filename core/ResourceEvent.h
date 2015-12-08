// ResourceEvent.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
#define _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_

#include "trip/client/core/DataId.h"

#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class Resource;

        struct ResourceChangedEvent
            : util::event::Event
        {
            Resource * resource;
            ResourceChangedEvent() : resource(NULL) {}
        };

        struct ResourceMeta;

        struct MetaChangedEvent
            : util::event::Event
        {
            ResourceMeta const * meta;
            MetaChangedEvent() : meta(NULL) {}
        };

        struct DataEvent
            : util::event::Event
        {
            DataId id;
            size_t degree;
            DataEvent() : degree(0) {}
        };

        struct SegmentMeta;

        struct SegmentMetaEvent
            : util::event::Event
        {
            DataId id;
            SegmentMeta const * meta;
            SegmentMetaEvent() : meta(NULL) {}
        };

        struct Source;

        struct SourceChangedEvent
            : util::event::Event
        {
            boost::uint32_t type; // 0 - add, 1 - del, 2 - modify
            Source const * source;
            SourceChangedEvent() : type(-1), source(NULL) {}
        };

        struct Sink;

        struct SinkChangedEvent
            : util::event::Event
        {
            boost::uint32_t type;
            Sink const * sink;
            SinkChangedEvent() : type(-1), sink(NULL) {}
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
