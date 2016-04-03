// Scheduler.h

#ifndef _TRIP_CLIENT_CORE_SCHEDULER_H_
#define _TRIP_CLIENT_CORE_SCHEDULER_H_

#include "trip/client/core/Resource.h"
#include "trip/client/core/Piece.h"

namespace trip
{
    namespace client
    {

        class Source;
        class Sink;

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

            virtual void add_sink(
                Sink * sink) = 0;

            virtual void del_sink(
                Sink * sink) = 0;

            virtual void update_sink(
                Sink * sink) = 0;

        public:
            virtual bool get_task(
                Source & source, 
                std::vector<DataId> & pieces) = 0;

			virtual void on_timeout(
				DataId const & piece) = 0;

            virtual void on_source_lost(
                Source & source) = 0;

        private:
            Resource & resource_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SCHEDULER_H_
