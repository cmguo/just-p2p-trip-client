// Piece.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Piece.h"

#include <framework/memory/PrivateMemory.h>

namespace trip
{
    namespace client
    {

        framework::memory::SmallFixedPool Piece::pool(framework::memory::PrivateMemory(), -1, sizeof(Piece));

        framework::memory::BigFixedPool Piece2::pool(framework::memory::PrivateMemory(), -1, sizeof(Piece2));

    } // namespace client
} // namespace trip
