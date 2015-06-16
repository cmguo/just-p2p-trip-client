// Struct.h

#ifndef _TRIP_EXPORT_DETAIL_STRUCT_H_
#define _TRIP_EXPORT_DETAIL_STRUCT_H_

#include "trip/client/export/detail/Preprocessor.h"

#define STRUCT_FIELD_TYPE_NAME(i, t, n) t n;

#define STRUCT_FIELDS_TYPE_NAME(np, params) LIST_PAIR_FORMAT(STRUCT_FIELD_TYPE_NAME, np, params)

#define TRIP_STRUCT_0(name, fields) TRIP_STRUCT(name, 0, fields)
#define TRIP_STRUCT_1(name, fields) TRIP_STRUCT(name, 1, fields)
#define TRIP_STRUCT_2(name, fields) TRIP_STRUCT(name, 2, fields)
#define TRIP_STRUCT_3(name, fields) TRIP_STRUCT(name, 3, fields)
#define TRIP_STRUCT_4(name, fields) TRIP_STRUCT(name, 4, fields)
#define TRIP_STRUCT_5(name, fields) TRIP_STRUCT(name, 5, fields)
#define TRIP_STRUCT_6(name, fields) TRIP_STRUCT(name, 6, fields)
#define TRIP_STRUCT_7(name, fields) TRIP_STRUCT(name, 7, fields)
#define TRIP_STRUCT_8(name, fields) TRIP_STRUCT(name, 8, fields)
#define TRIP_STRUCT_9(name, fields) TRIP_STRUCT(name, 9, fields)
#define TRIP_STRUCT_10(name, fields) TRIP_STRUCT(name, 10, fields)
#define TRIP_STRUCT_11(name, fields) TRIP_STRUCT(name, 11, fields)
#define TRIP_STRUCT_12(name, fields) TRIP_STRUCT(name, 12, fields)

#ifndef TRIP_STRUCT
#  define TRIP_STRUCT(name, nf, fields) typedef struct name { STRUCT_FIELDS_TYPE_NAME(nf, fields) } name
#endif

#endif // _TRIP_EXPORT_DETAIL_STRUCT_H_
