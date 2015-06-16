// CxxValueBuffer.h

#ifndef _TRIP_EXPORT_DETAIL_CXX_VALUE_BUFFER_H_
#define _TRIP_EXPORT_DETAIL_CXX_VALUE_BUFFER_H_

#include "trip/export/detail/CxxTypeBind.h"

#include <framework/container/Array.h>

#include <util/stream/StreamBuffers.h>

namespace trip
{

    template <>
    struct TypeBind<TRIP_ConstBuffer>
    {
        typedef TRIP_ConstBuffer ctype_t;
        typedef boost::asio::const_buffer xtype_t;

        static xtype_t c2x(ctype_t c)
        {
            return xtype_t(c.data, c.len);
        }

        static ctype_t x2c(xtype_t x)
        {
            ctype_t c;
            c.data = boost::asio::buffer_cast<PP_ubyte const *>(x);
            c.len = boost::asio::buffer_size(x);
            return c;
        }
    };
    
    struct buffers_holder
        : TRIP_ConstBuffers
    {
    public:
        buffers_holder(
            util::stream::StreamConstBuffers const & buffers)
        {
            util::stream::StreamConstBuffers::const_iterator iter = buffers.begin();
            for (; iter != buffers.end(); ++iter) {
                buffers_.push_back(TypeBind<TRIP_ConstBuffer>::x2c(*iter));
            }
            TRIP_ConstBuffers::buffers = &buffers_.front();
            TRIP_ConstBuffers::count = buffers_.size();
        }

    protected:
        std::vector<TRIP_ConstBuffer> buffers_;
    };

    template <>
    struct TypeBind<TRIP_ConstBuffers>
    {
        typedef TRIP_ConstBuffers ctype_t;
        typedef util::stream::StreamConstBuffers xtype_t;

        static xtype_t c2x(ctype_t c)
        {
            xtype_t x;
            for (PP_uint i = 0; i < c.count; ++i) {
                x.push_back(TypeBind<TRIP_ConstBuffer>::c2x(c.buffers[i]));
            }
            return x;
        }

        static ctype_t x2c(xtype_t x)
        {
            return buffers_holder(x);
        }
    };
    
} // namespace trip

#endif // _TRIP_EXPORT_DETAIL_CXX_VALUE_BUFFER_H_
