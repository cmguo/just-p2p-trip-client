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

        public:
            Url const & url() const;

            bool has_segment(
                DataId sid) const;

            bool has_block(
                DataId bid) const;

        public:
            bool request(
                std::vector<DataId> & pieces);

        protected:
            void on_ready();

            void on_map(
                DataId id, 
                boost::dynamic_bitset<> & map);

            void on_data(
                DataId id, 
                Piece::pointer piece);

        private:
            virtual bool open() = 0;

            virtual bool close() = 0;

            virtual bool do_request() = 0;

        protected:
            Resource & resource_;
            Url const url_;
            std::vector<DataId> requests_;

        private:
            Scheduler * scheduler_;
            DataId map_id_;
            boost::dynamic_bitset<> map_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SOURCE_H_
