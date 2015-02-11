// ResourceEvent.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
#define _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_

#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        struct ResourceMeta;

        struct MetaChangedEvent
            : util::event::Event
        {
            ResourceMeta const * meta;
        };

        struct DataEvent
            : util::event::Event
        {
            boost::uint64_t id;
        };

        struct SegmentMetaEvent
            : util::event::Event
        {
            boost::uint64_t id;
            SegmentMeta const * meta;
        };

        struct Source;

        struct SourceChangedEvent
            : util::event::Event
        {
            boost::uint32_t type;
            Source const * source;
        };

        struct Sink;

        struct SinkChangedEvent
            : util::event::Event
        {
            boost::uint32_t type;
            Sink const * sink;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
