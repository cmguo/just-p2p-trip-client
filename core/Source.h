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

        class Resource;

        class Source
        {
        public:
            Source(
                Resource & resource);

            virtual ~Source();

        public:
            virtual bool request(
                std::vector<boost::uint64_t> & pieces) = 0;

        protected:
            void on_data(
                boost::uint64_t id, 
                Piece::pointer piece);

            boost::uint16_t piece_size(
                boost::uint64_t id) const;

        private:
            Resource & resource_;
            boost::dynamic_bitset<> map_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_SOURCE_H_
