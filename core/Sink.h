// Sink.h

#ifndef _TRIP_CLIENT_CORE_SINK_H_
#define _TRIP_CLIENT_CORE_SINK_H_

#include "trip/client/core/Piece.h"

namespace trip
{
    namespace client
    {

        class Resource;

        struct SinkRequest
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

        protected:
            void seek_to(
                boost::uint64_t seg, 
                boost::uint32_t begin, 
                boost::uint32_t end);

            Piece::pointer read();

            bool bump();

        private:
            virtual size_t write(
                Piece * p, 
                boost::system::error_code & ec);

        private:

        private:
            Resource & resource_;
            SinkRequest type_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SINK_H_
