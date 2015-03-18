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
            virtual void add_source(
                Source * source);

            virtual bool get_task(
                Source & source, 
                std::vector<boost::uint64_t> & pieces) = 0;

        private:
            Resource & resource_;
            std::vector<Source *> sources_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SCHEDULER_H_
