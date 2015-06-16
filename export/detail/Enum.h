// Enum.h

#ifndef _TRIP_EXPORT_DETAIL_ENUM_H_
#define _TRIP_EXPORT_DETAIL_ENUM_H_

#include "trip/client/export/detail/Preprocessor.h"

#define ENUM_NAME_VALUE(i, n, v) n = v,
#define ENUMS_NAME_VALUE(ne, enums) LIST_PAIR_FORMAT(ENUM_NAME_VALUE, ne, enums)

#define ENUM_PREFIX_NAME_VALUE(i, p, n, v) BOOST_PP_CAT(BOOST_PP_CAT(p, _), n) = v,
#define ENUMS_PREFIX_NAME_VALUE(p, ne, enums) LIST_DATA_PAIR_FORMAT(ENUM_PREFIX_NAME_VALUE, p, ne, enums)

#define TRIP_ENUM_0(name, enums) TRIP_ENUM(name, 0, enums)
#define TRIP_ENUM_1(name, enums) TRIP_ENUM(name, 1, enums)
#define TRIP_ENUM_2(name, enums) TRIP_ENUM(name, 2, enums)
#define TRIP_ENUM_3(name, enums) TRIP_ENUM(name, 3, enums)
#define TRIP_ENUM_4(name, enums) TRIP_ENUM(name, 4, enums)
#define TRIP_ENUM_5(name, enums) TRIP_ENUM(name, 5, enums)
#define TRIP_ENUM_6(name, enums) TRIP_ENUM(name, 6, enums)
#define TRIP_ENUM_7(name, enums) TRIP_ENUM(name, 7, enums)
#define TRIP_ENUM_8(name, enums) TRIP_ENUM(name, 8, enums)
#define TRIP_ENUM_9(name, enums) TRIP_ENUM(name, 9, enums)
#define TRIP_ENUM_10(name, enums) TRIP_ENUM(name, 10, enums)
#define TRIP_ENUM_11(name, enums) TRIP_ENUM(name, 11, enums)

#ifndef TRIP_ENUM
#  ifdef __cplusplus
#    define TRIP_ENUM(name, ne, enums) typedef struct name { enum Enum { ENUMS_NAME_VALUE(ne, enums) }; } name
#  else
#    define TRIP_ENUM(name, ne, enums) typedef enum name ## _ ## Enum { ENUMS_PREFIX_NAME_VALUE(name, ne, enums) } name ## _ ## Enum
#  endif
#endif

#endif // _TRIP_EXPORT_DETAIL_ENUM_H_
