// Source.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        Source::Source(
            Resource & resource)
            : resource_(resource)
        {
        }

        Source::~Source()
        {
        }

        void Source::on_data(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            resource_.set_piece(id, piece);
        }

        boost::uint16_t Source::piece_size(
            boost::uint64_t id) const
        {
            return resource_.piece_size(id);
        }

    } // namespace client
} // namespace trip
