// CxxValueUrl.h

#ifndef _TRIP_EXPORT_DETAIL_CXX_VALUE_URL_H_
#define _TRIP_EXPORT_DETAIL_CXX_VALUE_URL_H_

#include "trip/export/detail/CxxTypeBind.h"

#include <framework/string/Url.h>

namespace trip
{

    struct url_holder
        : TRIP_Url
    {
    public:
        url_holder(
            framework::string::Url const & url)
            : str_(url.to_string())
        {
            TRIP_Url::str = str_.c_str();
        }

    protected:
        std::string str_;
    };

    template <>
    struct TypeBind<TRIP_Url>
    {
        typedef TRIP_Url ctype_t;
        typedef framework::string::Url xtype_t;

        static xtype_t c2x(ctype_t c)
        {
            return xtype_t(c.str);
        }

        static url_holder x2c(xtype_t x)
        {
            return url_holder(x);
        }

    protected:
        xtype_t x_;
        TRIP_Url c_;
    };
    
} // namespace trip

#endif // _TRIP_EXPORT_DETAIL_CXX_VALUE_URL_H_
