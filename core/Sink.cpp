// Sink.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Sink.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        Sink::Sink(
            Resource & resource)
            : resource_(resource)
        {
        }

        Sink::~Sink()
        {
        }

    } // namespace client
} // namespace trip
