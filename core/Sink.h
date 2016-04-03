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

        class Scheduler;

        class Sink
        {
        public:
            Sink(
                Resource & resource, 
                Url const & url);

            virtual ~Sink();

        public:
            Resource const & resource() const;

            Url const & url() const;

        public:
            boost::uint32_t type() const;

            DataId const & position() const;

        public:
            void attach(
                Scheduler & scheduler);

            void detach();

            void set_error(
                boost::system::error_code const & ec);

            bool attached() const;

            void close();

        protected:
            void seek_to(
                boost::uint64_t seg, 
                boost::uint32_t begin, 
                boost::uint32_t end);

            void seek_end(
                boost::uint64_t seg);

            bool at_end() const;

            Piece::pointer read();

        public:
            ResourceMeta const * meta();

            boost::system::error_code error();

            SegmentMeta const * segment_meta(
                boost::uint64_t segm);

        private:
            virtual void on_attach() {};

            virtual void on_error(
                boost::system::error_code const & ec) {};

            virtual void on_segment_meta(
                boost::uint64_t seg,  
                SegmentMeta const & meta) {};

            virtual void on_data() {};

        private:
            void on_event(
                util::event::Event const & event);

        protected:
            Resource * resource_;
            Url const url_;

        private:
            Scheduler * scheduler_;
            boost::system::error_code error_;
            //boost::uint32_t type_;
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
