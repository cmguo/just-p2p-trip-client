// Source.h

#ifndef _TRIP_CLIENT_CORE_SOURCE_H_
#define _TRIP_CLIENT_CORE_SOURCE_H_

#include "trip/client/core/Piece.h"

#include <framework/string/Url.h>

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
                Scheduler & scheduler);

            virtual ~Source();

        public:
            Resource const & resource() const;

        public:
            virtual bool request(
                std::vector<boost::uint64_t> & pieces) = 0;

        protected:
            void on_ready();

            void on_data(
                boost::uint64_t id, 
                Piece::pointer piece);

        private:
            Scheduler & scheduler_;
            boost::dynamic_bitset<> map_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SOURCE_H_
