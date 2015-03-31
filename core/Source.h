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

            Url const & url() const;

            bool request(
                std::vector<boost::uint64_t> & pieces);

        protected:
            void on_ready();

            void on_data(
                boost::uint64_t id, 
                Piece::pointer piece);

        private:
            virtual bool open() = 0;

            virtual bool close() = 0;

            virtual bool do_request() = 0;

        protected:
            Resource & resource_;
            Url const url_;
            std::vector<boost::uint64_t> requests_;

        private:
            Scheduler * scheduler_;
            boost::dynamic_bitset<> map_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SOURCE_H_
