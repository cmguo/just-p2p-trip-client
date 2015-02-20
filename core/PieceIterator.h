// PieceIterator.h

#ifndef _TRIP_CLIENT_CORE_PIECE_ITERATOR_H_
#define _TRIP_CLIENT_CORE_PIECE_ITERATOR_H_

#include "trip/client/core/Piece.h"

#include <boost/iterator/iterator_facade.hpp>

namespace trip
{
    namespace client
    {

        class ResourceData;
        class Segment;
        class Block;

        class PieceIterator
            : public boost::iterator_facade<
                PieceIterator,
                Piece::pointer const,
                boost::forward_traversal_tag
              >
        {
        public:
            PieceIterator(
                ResourceData & resource, 
                boost::uint64_t id)
                : resource_(&resource)
                , id_(id)
                , segment_(NULL)
                , block_(NULL)
            {
            }

        public:
            boost::uint64_t id() const
            {
                return id_;
            }

        private:
            friend class boost::iterator_core_access;

            void increment();

            bool equal(
                PieceIterator const & r) const
            {
                assert(r.resource_ == resource_);
                return id_ == r.id_;
            }

            reference dereference() const
            {
                return piece_;
            }

        private:
            friend class ResourceData;

            ResourceData * resource_;
            boost::uint64_t id_;
            Segment const * segment_;
            Block const * block_;
            Piece::pointer piece_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_PIECE_H_
