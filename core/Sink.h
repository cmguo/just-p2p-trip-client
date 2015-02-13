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
            SinkRequest const & get_request() const
            {
                return request_;
            }

        public:
            bool get_meta(
                ResourceMeta & meta,
                boost::system::error_code & ec);

            bool get_segment_meta(
                SegmentMeta & meta, 
                boost::system::error_code & ec);

            bool get_stat(
                ResourceStat & stat, 
                boost::system::error_code & ec);

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
                boost::system::error_code & ec) = 0;

        private:

        private:
            Resource & resource_;
            SinkRequest request_;
            PieceIterator iter_;
            PieceIterator end_;
            boost::uint32_t off_;
            boost::uint32_t size_; // left
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SINK_H_
