// Scheduler.h

#ifndef _TRIP_CLIENT_CORE_SCHEDULER_H_
#define _TRIP_CLIENT_CORE_SCHEDULER_H_

#include "trip/client/core/Piece.h"

namespace trip
{
    namespace client
    {

        class Resource;
        class Source;

        class Scheduler
        {
        public:
            Scheduler(
                Resource & resource);

            virtual ~Scheduler();

        public:
            Resource & resource()
            {
                return resource_;
            }

        public:
            virtual bool get_map_range(
                DataId & begin, 
                size_t count) = 0;

            virtual bool get_task(
                Source & source, 
                std::vector<DataId> & pieces) = 0;

        private:
            Resource & resource_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SCHEDULER_H_
