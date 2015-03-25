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
            Resource & resource)
            : resource_(resource)
        {
        }

        Source::~Source()
        {
        }

        void Source::attach(
            Scheduler & scheduler)
        {
            scheduler_ = &scheduler;
        }

        void Source::on_ready()
        {
            std::vector<boost::uint64_t> pieces;
            if (scheduler_->get_task(*this, pieces))
                request(pieces);
        }

        void Source::on_data(
            boost::uint64_t id, 
            Piece::pointer piece)
        {
            resource_.set_piece(id, piece);
            on_ready();
        }

    } // namespace client
} // namespace trip
