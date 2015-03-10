// TimeEvent.h

#ifndef _TRIP_CLIENT_TIMER_TIME_EVENT_H_
#define _TRIP_CLIENT_TIMER_TIME_EVENT_H_

#include <util/event/Event.h>

#include <framework/timer/ClockTime.h>

namespace trip
{
    namespace client
    {

        struct TimeEvent
            : util::event::Event
        {
            framework::timer::Time now;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_TIMER_TIME_EVENT_H_
