// Scheduler.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Scheduler.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        Scheduler::Scheduler(
            Resource & resource)
            : resource_(resource)
        {
        }

        Scheduler::~Scheduler()
        {
        }

    } // namespace client
} // namespace trip
