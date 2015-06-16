// Interface.h

#ifndef _TRIP_EXPORT_DETAIL_INTERFACE_H_
#define _TRIP_EXPORT_DETAIL_INTERFACE_H_

#include "trip/export/detail/Preprocessor.h"

#define METHOD_PARAM_TYPE_NAME(i, t, n) BOOST_PP_COMMA_IF(i) t n
#define METHOD_PARAM_TYPE(i, t, n) BOOST_PP_COMMA_IF(i) t
#define METHOD_PARAM_NAME(i, t, n) BOOST_PP_COMMA_IF(i) n

#define METHOD_PARAMS_TYPE_NAME(np, params) LIST_PAIR_FORMAT(METHOD_PARAM_TYPE_NAME, np, params)
#define METHOD_PARAMS_TYPE(np, params) LIST_PAIR_FORMAT(METHOD_PARAM_TYPE, np, params)
#define METHOD_PARAMS_NAME(np, params) LIST_PAIR_FORMAT(METHOD_PARAM_NAME, np, params)

#define INTERFACE_METHOD(i, m) m
#define INTERFACE_METHODS(nm, methods) LIST_FORMAT(INTERFACE_METHOD, nm, methods)

#ifndef TRIP_INTERFACE
#  ifdef __cplusplus
#    define TRIP_METHOD(type, name, np, params) \
        type (*name)(TRIP_Plugin * BOOST_PP_COMMA_IF(np) METHOD_PARAMS_TYPE(np, params));
#    define TRIP_INTERFACE(name, base, nm, methods) \
        struct name : base { \
            INTERFACE_METHODS(nm, methods) \
        };
#  else
#  endif
#endif

#endif // _TRIP_EXPORT_DETAIL_INTERFACE_H_
