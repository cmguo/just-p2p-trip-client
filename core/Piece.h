// Piece.h

#ifndef _TRIP_CLIENT_CORE_PIECE_H_
#define _TRIP_CLIENT_CORE_PIECE_H_

namespace trip
{
    namespace client
    {

#define PIECE_SIZE 1400
#define PIECE_PER_BLOCK 64
#define BLOCK_SIZE (PIECE_SIZE * PIECE_PER_BLOCK)
#define BLOCK_PER_SEGMENT 64
#define SEGMENT_SIZE (BLOCK_SIZE * BLOCK_PER_SEGMENT)

#define SEGMENT(id) (id >> 12)
#define BLOCK(id) boost::uint16_t((id >> 6) & 0x3f)
#define PIECE(id) boost::uint16_t(id & 0xfff)
#define BLOCK_PIECE(id) boost::uint32_t(id & 0xffffff)

#define MAX_PIECE boost::uint16_t(0xfff)
#define MAX_BLOCK boost::uint16_t(0xfff)
#define MAX_SEGMENT boost::uint64_t(0xffffffffffULL)

#define MAKE_ID(s, b, p) ((s << 12) | (b << 6) | p)

        class Piece
        {
        public:
            typedef boost::intrusive_ptr<Piece> pointer;

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
                boost::uint8_t * data, 
                boost::uint16_t size)
                : nref_(1)
                , reserved_(0)
                , size_(size)
                , data_(data)
            {
            }

            ~Piece() {}

            void set_size(
                boost::uint16_t size)
            {
                assert(size < size_);
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
                if (--p->nref_) {
                    Piece::free(p);
                }
            }

        private:
            boost::uint8_t nref_;
            boost::uint8_t reserved_;
            boost::uint16_t size_;
            boost::uint8_t * data_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_PIECE_H_
