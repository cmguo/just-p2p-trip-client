// CxxValue.h

#ifndef _TRIP_CLIENT_EXPORT_DETAIL_CXX_VALUE_H_
#define _TRIP_CLIENT_EXPORT_DETAIL_CXX_VALUE_H_

#include "trip/client/export/detail/CxxTypeBind.h"

namespace trip
{

    template <
        typename cT
    >
    struct XValue
    {
        typedef cT ctype_t;
        typedef typename TypeBind<cT>::xtype_t xtype_t;

        XValue(
            xtype_t x)
            : x_(x)
        {
        }

        ctype_t cvalue() const
        {
            return TypeBind<cT>::x2c(x_);
        }

    protected:
        xtype_t x_;
    };

    template <
        typename cT
    >
    struct XValue<cT *>
    {
        typedef cT * ctype_t;
        typedef typename TypeBind<cT>::xtype_t & xtype_t;

        XValue(
            xtype_t x)
            : x_(x)
            , c_(TypeBind<cT>::x2c(x))
        {
        }

        ~XValue()
        {
            x_ = TypeBind<cT>::c2x(c_);
        }

        ctype_t cvalue()
        {
            return &c_;
        }

    protected:
        xtype_t x_;
        cT c_;
    };

    template <
        typename cT
    >
    struct XValue<cT const *>
    {
        typedef cT const * ctype_t;
        typedef typename TypeBind<cT>::xtype_t const & xtype_t;

        XValue(
            xtype_t x)
            : x_(x)
            , c_(TypeBind<cT>::x2c(x))
        {
        }

        ctype_t cvalue() const
        {
            return &c_;
        }

    protected:
        xtype_t x_;
        cT const c_;
    };

    template <
        typename cT
    >
    struct CValue
    {
        typedef cT ctype_t;
        typedef typename TypeBind<cT>::xtype_t xtype_t;

        CValue(
            ctype_t c)
            : c_(c)
        {
        }

        xtype_t xvalue() const
        {
            return TypeBind<cT>::c2x(c_);
        }

    protected:
        ctype_t c_;
    };

    template <
        typename cT
    >
    struct CValue<cT *>
    {
        typedef cT * ctype_t;
        typedef typename TypeBind<cT>::xtype_t & xtype_t;

        CValue(
            ctype_t c)
            : c_(c)
            , x_(TypeBind<cT>::c2x(c))
        {
        }

        ~CValue()
        {
            *c_ = TypeBind<cT>::x2c(x_);
        }

        xtype_t xvalue()
        {
            return x_;
        }

    protected:
        ctype_t c_;
        typename TypeBind<cT>::xtype_t x_;
    };

    template <
        typename cT
    >
    struct CValue<cT const *>
    {
        typedef cT const * ctype_t;
        typedef typename TypeBind<cT>::xtype_t const & xtype_t;

        CValue(
            ctype_t c)
            : c_(c)
        {
        }

        typename TypeBind<cT>::xtype_t const xvalue() const
        {
            return TypeBind<cT>::c2x(*c_);
        }

    protected:
        cT const c_;
    };

} // namespace trip

#endif // _TRIP_CLIENT_EXPORT_DETAIL_CXX_VALUE_H_
