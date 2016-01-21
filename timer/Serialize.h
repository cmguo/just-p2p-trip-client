// Serialize.h

#ifndef _TRIP_CLIENT_TIMER_SERIALIZE_H_
#define _TRIP_CLIENT_TIMER_SERIALIZE_H_

#include "trip/client/main/TimerManager.h"

namespace trip
{
    namespace client
    {

        template <typename Archive>
        void serialize(
            Archive & ar, 
            TimerManager & t)
        {
        }

    }
}

namespace util
{
    namespace serialization
    {

    }
}

#endif // _TRIP_CLIENT_TIMER_SERIALIZE_H_
