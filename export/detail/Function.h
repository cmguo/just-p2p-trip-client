// Function.h

#ifndef _TRIP_EXPORT_DETAIL_FUNCTION_H_
#define _TRIP_EXPORT_DETAIL_FUNCTION_H_

#include "trip/client/export/detail/Preprocessor.h"

#define FUNCTION_PARAM_TYPE_NAME(i, t, n) BOOST_PP_COMMA_IF(i) t n
#define FUNCTION_PARAM_TYPE(i, t, n) BOOST_PP_COMMA_IF(i) t
#define FUNCTION_PARAM_NAME(i, t, n) BOOST_PP_COMMA_IF(i) n

#define FUNCTION_PARAMS_TYPE_NAME(np, params) LIST_PAIR_FORMAT(FUNCTION_PARAM_TYPE_NAME, np, params)
#define FUNCTION_PARAMS_TYPE(np, params) LIST_PAIR_FORMAT(FUNCTION_PARAM_TYPE, np, params)
#define FUNCTION_PARAMS_NAME(np, params) LIST_PAIR_FORMAT(FUNCTION_PARAM_NAME, np, params)

#ifndef TRIP_DECL
#  define TRIP_DECL
#endif

#define TRIP_FUNC_0(type, name, params) TRIP_FUNC(type, name, 0, params)
#define TRIP_FUNC_1(type, name, params) TRIP_FUNC(type, name, 1, params)
#define TRIP_FUNC_2(type, name, params) TRIP_FUNC(type, name, 2, params)
#define TRIP_FUNC_3(type, name, params) TRIP_FUNC(type, name, 3, params)
#define TRIP_FUNC_4(type, name, params) TRIP_FUNC(type, name, 4, params)
#define TRIP_FUNC_5(type, name, params) TRIP_FUNC(type, name, 5, params)
#define TRIP_FUNC_6(type, name, params) TRIP_FUNC(type, name, 6, params)
#define TRIP_FUNC_7(type, name, params) TRIP_FUNC(type, name, 7, params)

#ifndef TRIP_FUNC
#  ifdef __cplusplus
#    define TRIP_FUNC(type, name, np, params) extern "C" { TRIP_DECL type name(FUNCTION_PARAMS_TYPE(np, params)); }
#  else
#    define TRIP_FUNC(type, name, np, params) TRIP_DECL type name(FUNCTION_PARAMS_TYPE(np, params))
#  endif
#endif

#endif // _TRIP_EXPORT_DETAIL_FUNCTION_H_
