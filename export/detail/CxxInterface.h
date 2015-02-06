// Interface.h

#include "trip/export/detail/IPreprocessor.h"
#include "trip/export/detail/CxxValue.h"

#define PARAM_CXX_TYPE_NAME(i, t, n) BOOST_PP_COMMA_IF(i) TypeBind<t>::xtype_t n
#define PARAM_CXX_TYPE(i, t, n) BOOST_PP_COMMA_IF(i) TypeBind<t>::xtype_t
#define PARAM_LOCAL_VARIABLE(i, t, n) XValue<t> BOOST_PP_CAT(n, _l)(n);
#define PARAM_LOCAL_VALUE(i, t, n) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(n, _l).cvalue()

#define PARAMS_CXX_TYPE_NAME(np, params) LIST_PAIR_FORMAT(PARAM_CXX_TYPE_NAME, np, params)
#define PARAMS_CXX_TYPE(np, params) LIST_PAIR_FORMAT(PARAM_CXX_TYPE, np, params)
#define PARAMS_LOCAL_VARIABLE(np, params) LIST_PAIR_FORMAT(PARAM_LOCAL_VARIABLE, np, params)
#define PARAMS_LOCAL_VALUE(np, params) LIST_PAIR_FORMAT(PARAM_LOCAL_VALUE, np, params)

#undef TRIP_METHOD
#define TRIP_METHOD(type, name, np, params) \
    virtual TypeBind<type>::xtype_t name(PARAMS_CXX_TYPE_NAME(np, params)) { \
        PARAMS_LOCAL_VARIABLE(np, params); \
        return CValue<type>(impl().name(pimpl() BOOST_PP_COMMA_IF(np) PARAMS_LOCAL_VALUE(np, params))).xvalue(); \
    }

#undef TRIP_INTERFACE
#define TRIP_INTERFACE(name, base, nm, methods) \
        INTERFACE_METHODS(nm, methods)
