// BlockData.h

#ifndef _TRIP_CLIENT_CORE_BLOCK_DATA_H_
#define _TRIP_CLIENT_CORE_BLOCK_DATA_H_

#include "trip/client/core/Piece.h"

#include <boost/filesystem/path.hpp>

namespace trip
{
    namespace client
    {

        class Block;

        class BlockData
        {
        private:
            BlockData(
                void * map_addr, 
                boost::uint32_t offset,
                boost::uint32_t size);

            ~BlockData();

        public:
            static BlockData * alloc(
                boost::filesystem::path const & path, 
                boost::uint32_t offset, 
                boost::uint32_t size);

            static void free(
                BlockData * p);

            friend void intrusive_ptr_add_ref(
                BlockData * p)
            {
                ++p->nref_;
            }

            friend void intrusive_ptr_release(
                BlockData * p)
            {
                if (--p->nref_) {
                    BlockData::free(p);
                }
            }

        private:
            friend class Block;

            size_t nref_;
            void * map_addr_;
            boost::uint32_t offset_;
            boost::uint32_t size_;
        };

        class BlockPiece
            : public Piece
        {
        public:
            static void set_capacity(
                size_t capacity);

            static BlockPiece * alloc(
                boost::intrusive_ptr<BlockData> block, 
                boost::uint8_t * data, 
                boost::uint16_t size);

        private:
            friend class Piece;

            static void free(
                BlockPiece * p);

        private:
            BlockPiece(
                boost::intrusive_ptr<BlockData> block, 
                boost::uint8_t * data, 
                boost::uint16_t size)
                : Piece(data, size)
                , block_(block)
            {
            }

            ~BlockPiece() {}

        private:
            boost::intrusive_ptr<BlockData> block_;
        };

    } // namespace client
} // namespace trip

#endif // _TRIP_CLIENT_CORE_BLOCK_DATA_H_
