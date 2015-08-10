// Piece.h

#ifndef _TRIP_CLIENT_CORE_PIECE_H_
#define _TRIP_CLIENT_CORE_PIECE_H_

#include "trip/client/core/DataId.h"

#include <boost/intrusive_ptr.hpp>

namespace trip
{
    namespace client
    {

#define PIECE_SIZE 1400
#define PIECE_PER_BLOCK (1 << PIECE_BIT)
#define BLOCK_SIZE (PIECE_SIZE * PIECE_PER_BLOCK)
#define BLOCK_PER_SEGMENT (1 << BLOCK_BIT)
#define SEGMENT_SIZE (BLOCK_SIZE * BLOCK_PER_SEGMENT)

        class Piece
        {
        public:
            typedef boost::intrusive_ptr<Piece> pointer;

            boost::uint8_t nref() const
            {
                return nref_;
            }

            boost::uint8_t * data() const
            {
                return data_;
            }

            boost::uint16_t size() const
            {
                return size_;
            }

        protected:
            Piece(
                boost::uint8_t type,
                boost::uint8_t * data, 
                boost::uint16_t size)
                : nref_(0)
                , type_(type)
                , size_(size)
                , data_(data)
            {
            }

            ~Piece() {}

            void set_size(
                boost::uint16_t size)
            {
                assert(size <= size_);
                size_ = size;
            }

        private:
            static void free(
                Piece * p);

            friend void intrusive_ptr_add_ref(
                Piece * p)
            {
                ++p->nref_;
            }

            friend void intrusive_ptr_release(
                Piece * p)
            {
                if (--p->nref_ == 0) {
                    Piece::free(p);
                }
            }

        private:
            boost::uint8_t nref_;
            boost::uint8_t type_;
            boost::uint16_t size_;
            boost::uint8_t * data_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_PIECE_H_
