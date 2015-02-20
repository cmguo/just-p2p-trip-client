// Sink.h

#ifndef _TRIP_CLIENT_CORE_SINK_H_
#define _TRIP_CLIENT_CORE_SINK_H_

#include "trip/client/core/Piece.h"
#include "trip/client/core/PieceIterator.h"

#include <util/event/Event.h>

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
            SinkRequest const & get_request() const
            {
                return request_;
            }

            Resource const & resource() const
            {
                return resource_;
            }

        protected:
            void seek_to(
                boost::uint64_t seg, 
                boost::uint32_t begin, 
                boost::uint32_t end);

            bool at_end() const;

            Piece::pointer read();

        private:
            virtual void on_data() = 0;

        private:
            void on_event(
                util::event::Event const & event);

        protected:
            Resource & resource_;

        private:
            SinkRequest request_;
            PieceIterator iter_;
            PieceIterator end_;
            boost::uint32_t off_;
            boost::uint32_t size_; // left
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SINK_H_
