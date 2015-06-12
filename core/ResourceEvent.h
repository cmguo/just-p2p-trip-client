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
        };

        struct ResourceMeta;

        struct MetaChangedEvent
            : util::event::Event
        {
            ResourceMeta const * meta;
        };

        struct DataEvent
            : util::event::Event
        {
            DataId id;
        };

        struct SegmentMeta;

        struct SegmentMetaEvent
            : util::event::Event
        {
            DataId id;
            SegmentMeta const * meta;
        };

        struct Source;

        struct SourceChangedEvent
            : util::event::Event
        {
            boost::uint32_t type; // 0 - add, 1 - del, 2 - modify
            Source const * source;
        };

        struct Sink;

        struct SinkChangedEvent
            : util::event::Event
        {
            boost::uint32_t type;
            Sink const * sink;
        };

        struct OutOfMemoryEvent
            : util::event::Event
        {
            size_t level;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
