// Export.h

#ifdef BOOST_HAS_DECLSPEC
#  define TRIP_DECL __declspec(dllexport)
#else
#  define TRIP_DECL __attribute__ ((visibility("default")))
#endif

#include "trip/client/export/BoostTypes.h"
#include "trip/client/export/Client.h"
