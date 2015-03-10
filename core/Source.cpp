// Source.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Source.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Scheduler.h"

namespace trip
{
    namespace client
    {

        Source::Source(
            Scheduler & scheduler)
            : scheduler_(scheduler)
        {
        }

        Source::~Source()
        {
        }

        Resource const & Source::resource() const
        {
            return scheduler_.resource();
        }

        void Source::on_data(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            scheduler_.resource().set_piece(id, piece);
            std::vector<boost::uint64_t> pieces;
            if (scheduler_.get_task(*this, pieces))
                request(pieces);
        }

    } // namespace client
} // namespace trip
