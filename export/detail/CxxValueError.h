// CxxValueError.h

#ifndef _TRIP_EXPORT_DETAIL_CXX_VALUE_ERROR_H_
#define _TRIP_EXPORT_DETAIL_CXX_VALUE_ERROR_H_

#include "trip/export/detail/CxxTypeBind.h"

namespace trip
{

    template <>
    struct TypeBind<TRIP_Error>
    {
        typedef TRIP_Error ctype_t;
        typedef boost::system::error_code xtype_t;

        static xtype_t c2x(ctype_t c)
        {
            return xtype_t(c.ec, boost::system::get_system_category());
        }

        static ctype_t x2c(xtype_t x)
        {
            ctype_t c;
            c.ec = x.value();
            return c;
        }
    };
    
} // namespace trip

#endif // _TRIP_EXPORT_DETAIL_CXX_VALUE_ERROR_H_
