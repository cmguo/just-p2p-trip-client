// TripDynamic.h

#ifndef _TRIP_EXPORT_TRIP_DYNAMIC_H_
#define _TRIP_EXPORT_TRIP_DYNAMIC_H_

#ifdef WIN32
# define TRIP_DECL __declspec(dllimport)
#else
# define TRIP_DECL
#endif

#include "Client.h"

#endif // _TRIP_EXPORT_TRIP_DYNAMIC_H_
