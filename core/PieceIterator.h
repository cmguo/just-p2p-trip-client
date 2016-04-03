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
        class Segment2;
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
            PieceIterator()
                : resource_(NULL)
                , segment2_(NULL)
                , segment_(NULL)
                , block_(NULL)
            {
            }

        private:
            PieceIterator(
                ResourceData & resource, 
                DataId id)
                : resource_(&resource)
                , id_(id)
                , segment2_(NULL)
                , segment_(NULL)
                , block_(NULL)
            {
            }

        public:
            DataId const & id() const
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
            DataId id_;
            Segment2 * segment2_;
            Segment * segment_;
            Block * block_;
            Piece::pointer piece_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_PIECE_H_
