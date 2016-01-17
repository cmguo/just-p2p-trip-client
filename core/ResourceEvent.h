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

        struct ResourceChangedEvent
            : util::event::EventEx<ResourceChangedEvent>
        {
            Resource * resource;
            ResourceChangedEvent(char const * name) 
                : util::event::EventEx<ResourceChangedEvent>(name), resource(NULL) {}
        };

        struct ResourceMeta;

        struct MetaChangedEvent
            : util::event::EventEx<MetaChangedEvent>
        {
            ResourceMeta const * meta;
            MetaChangedEvent(char const * name) 
                : util::event::EventEx<MetaChangedEvent>(name), meta(NULL) {}
        };

        struct DataEvent
            : util::event::EventEx<DataEvent>
        {
            DataId id;
            size_t degree;
            DataEvent(char const * name) 
                : util::event::EventEx<DataEvent>(name), degree(0) {}
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
