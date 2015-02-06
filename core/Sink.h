// Sink.h

#ifndef _TRIP_CLIENT_CORE_SINK_H_
#define _TRIP_CLIENT_CORE_SINK_H_

#include "trip/client/core/Piece.h"

namespace trip
{
    namespace client
    {

        class Resource;

        struct SinkType
        {
            boost::uint32_t type;
            boost::uint64_t begin;
            boost::uint64_t end;
        };

        class Sink
        {
        public:
            Sink(
                Resource & resource);

            virtual ~Sink();

        public:
            virtual bool response(
                boost::uint64_t index, 
                Piece::pointer piece) = 0;

        public:

        private:
            Resource & resource_;
            SinkType type_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SINK_H_
