// TimeEvent.h

#ifndef _TRIP_CLIENT_TIMER_TIME_EVENT_H_
#define _TRIP_CLIENT_TIMER_TIME_EVENT_H_

#include <util/event/EventEx.h>

namespace trip
{
    namespace client
    {

        struct TimeEvent
            : util::event::EventEx<TimeEvent>
        {
            Time now;
            TimeEvent(char const * name) 
                : util::event::EventEx<TimeEvent>(name) {}
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_TIMER_TIME_EVENT_H_
