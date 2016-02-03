// Source.h

#ifndef _TRIP_CLIENT_CORE_SOURCE_H_
#define _TRIP_CLIENT_CORE_SOURCE_H_

#include "trip/client/core/Piece.h"

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

namespace trip
{
    namespace client
    {

        class Scheduler;
        class Resource;
        struct SegmentMeta;

        class Source
        {
        public:
            Source(
                Resource & resource,
                Url const & url);

            virtual ~Source();

        public:
            void attach(
                Scheduler & scheduler);

            void detach();

            bool attached() const;

            void context(
                void * ctx);

            void * context() const;

        public:
            Resource & resource() const;

            Url const & url() const;

        public:
            virtual bool has_segment(
                DataId sid) const = 0;

            virtual bool has_block(
                DataId bid) const = 0;

            virtual boost::uint32_t window_size() const = 0;

            virtual boost::uint32_t window_left() const = 0;

        public:
            virtual bool request(
                std::vector<DataId> const & pieces) = 0;

            virtual void cancel() = 0;

            virtual void seek(
                DataId id);

        protected:
            void on_ready();

            void on_segment_meta(
                DataId id, 
                SegmentMeta const & meta);

            void on_data(
                DataId id, 
                Piece::pointer piece);

            void on_timeout(
                DataId id);

        private:
            virtual bool open(
                Url const & url) = 0;

            virtual bool close() = 0;

        protected:
            Resource & resource_;
            Url const url_;

        private:
            Scheduler * scheduler_;
            void * ctx_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SOURCE_H_
