// ResourceEvent.h

#ifndef _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
#define _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_

#include "trip/client/core/DataId.h"

#include <util/event/EventEx.h>

namespace trip
{
    namespace client
    {

        class Resource;

        struct ResourceEvent
            : util::event::EventEx<ResourceEvent>
        {
            Resource * resource;
            ResourceEvent(char const * name) 
                : util::event::EventEx<ResourceEvent>(name), resource(NULL) {}
        };

        struct ResourceMeta;

        struct ResourceMetaEvent
            : util::event::EventEx<ResourceMetaEvent>
        {
            ResourceMeta const * meta;
            ResourceMetaEvent(char const * name) 
                : util::event::EventEx<ResourceMetaEvent>(name), meta(NULL) {}
        };

        struct ResourceErrorEvent
            : util::event::EventEx<ResourceErrorEvent>
        {
            boost::system::error_code ec;
            ResourceErrorEvent(char const * name) 
                : util::event::EventEx<ResourceErrorEvent>(name) {}
        };

        struct ResourceDataEvent
            : util::event::EventEx<ResourceDataEvent>
        {
            DataId id;
            size_t degree;
            ResourceDataEvent(char const * name) 
                : util::event::EventEx<ResourceDataEvent>(name), degree(0) {}
        };

        struct SegmentMeta;

        struct SegmentMetaEvent
            : util::event::EventEx<SegmentMetaEvent>
        {
            DataId id;
            SegmentMeta const * meta;
            SegmentMetaEvent(char const * name) 
                : util::event::EventEx<SegmentMetaEvent>(name), meta(NULL) {}
        };

        struct Source;

        struct SourceChangedEvent
            : util::event::EventEx<SourceChangedEvent>
        {
            boost::uint32_t type; // 0 - add, 1 - del, 2 - modify
            Source const * source;
            SourceChangedEvent(char const * name) 
                : util::event::EventEx<SourceChangedEvent>(name), type(-1), source(NULL) {}
        };

        struct Sink;

        struct SinkChangedEvent
            : util::event::EventEx<SourceChangedEvent>
        {
            boost::uint32_t type;
            Sink const * sink;
            SinkChangedEvent(char const * name) 
                : util::event::EventEx<SourceChangedEvent>(name), type(-1), sink(NULL) {}
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_RESOURCE_EVENT_H_
