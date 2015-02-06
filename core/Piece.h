// Piece.h

#ifndef _TRIP_CLIENT_CORE_PIECE_H_
#define _TRIP_CLIENT_CORE_PIECE_H_

#include <framework/memory/BigFixedPool.h>
#include <framework/memory/SmallFixedPool.h>
#include <framework/memory/MemoryPoolObject.h>

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

        struct DataStatus
        {
            enum {
                memory = 1, 
                disk = 2
            };
        };

        struct Piece
        {
            typedef boost::intrusive_ptr<Piece> pointer;

            boost::uint8_t nref;
            boost::uint8_t status;
            boost::uint16_t size;
            boost::uint8_t * data;

            static void set_capacity(
                size_t capacity);

            static Piece * alloc();

            static void free(
                Piece * p);

        protected:
            Piece()
                : nref(1)
                , status(0)
                , size(0)
            {
            }

            ~Piece() {}

        private:
            static framework::memory::SmallFixedPool pool;
        };

        struct Piece2
            : public Piece
        {
            boost::uint8_t buf[PIECE_SIZE];

            static void set_capacity(
                size_t capacity);

            static Piece2 * alloc();

            static void free(
                Piece2 * p);

        private:
            Piece2()
            {
                data = buf;
            }

            ~Piece2() {}

        private:
            static framework::memory::BigFixedPool pool;
        };

        inline Piece * Piece::alloc()
        {
            void * ptr = pool.alloc(sizeof(Piece));
            if (ptr)
                return new (ptr) Piece;
            else
                return NULL;
        }

        inline void Piece::free(
            Piece * p)
        {
            p->~Piece();
            pool.free(p);
        }

        inline Piece2 * Piece2::alloc()
        {
            void * ptr = pool.alloc(sizeof(Piece2));
            if (ptr)
                return new (ptr) Piece2;
            else
                return NULL;
        }

        inline void Piece2::free(
            Piece2 * p)
        {
            p->~Piece2();
            pool.free(p);
        }

        inline void intrusive_ptr_add_ref(
            Piece * p)
        {
            ++p->nref;
        }

        inline void intrusive_ptr_release(
            Piece * p)
        {
            if (--p->nref) {
                if (p->data == (boost::uint8_t *)(p + 1))
                    Piece2::free((Piece2 *)p);
                else
                    Piece::free(p);
            }
        }

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_PIECE_H_
