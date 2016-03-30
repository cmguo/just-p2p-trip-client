// Sink.h

#ifndef _TRIP_CLIENT_CORE_SINK_H_
#define _TRIP_CLIENT_CORE_SINK_H_

#include "trip/client/core/Resource.h"
#include "trip/client/core/Piece.h"
#include "trip/client/core/PieceIterator.h"

#include <util/event/Event.h>

namespace trip
{
    namespace client
    {

        class Sink
        {
        public:
            Sink(
                Resource & resource);

            virtual ~Sink();

        public:
            boost::uint32_t type() const;

            DataId position() const;

            Resource const & resource() const
            {
                return *resource_;
            }

        protected:
            void seek_to(
                boost::uint64_t seg, 
                boost::uint32_t begin, 
                boost::uint32_t end);

            void seek_end(
                boost::uint64_t seg);

            bool at_end() const;

            Piece::pointer read();

        private:
            virtual void on_meta(
                ResourceMeta const & meta) {};

            virtual void on_error(
                boost::system::error_code const & ec) {};

            virtual void on_segment_meta(
                boost::uint64_t seg,  
                SegmentMeta const & meta) {};

            virtual void on_data() {};

        private:
            void on_event(
                util::event::Event const & event);

        private:
            Resource * resource_;
            boost::uint32_t type_;
            PieceIterator beg_;
            PieceIterator pos_;
            PieceIterator end_;
            boost::uint32_t off_;
            boost::uint32_t off2_; // left
            Resource::lock_t lock_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SINK_H_
