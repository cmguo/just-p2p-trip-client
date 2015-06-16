// Callback.h

#ifndef _TRIP_EXPORT_DETAIL_CALLBACK_H_
#define _TRIP_EXPORT_DETAIL_CALLBACK_H_

#define TRIP_CALLBACK_0(type, name, fields) TRIP_CALLBACK(type, name, 0, fields)
#define TRIP_CALLBACK_1(type, name, fields) TRIP_CALLBACK(type, name, 1, fields)
#define TRIP_CALLBACK_2(type, name, fields) TRIP_CALLBACK(type, name, 2, fields)
#define TRIP_CALLBACK_3(type, name, fields) TRIP_CALLBACK(type, name, 3, fields)
#define TRIP_CALLBACK_4(type, name, fields) TRIP_CALLBACK(type, name, 4, fields)
#define TRIP_CALLBACK_5(type, name, fields) TRIP_CALLBACK(type, name, 5, fields)
#define TRIP_CALLBACK_6(type, name, fields) TRIP_CALLBACK(type, name, 6, fields)
#define TRIP_CALLBACK_7(type, name, fields) TRIP_CALLBACK(type, name, 7, fields)

#ifndef TRIP_CALLBACK
#  define TRIP_CALLBACK(type, name, np, params) typedef type (*name)params
#endif

#endif // _TRIP_EXPORT_DETAIL_CALLBACK_H_
