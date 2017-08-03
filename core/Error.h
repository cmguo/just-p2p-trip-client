// Error.h

#ifndef _TRIP_CORE_ERROR_H_
#define _TRIP_CORE_ERROR_H_

namespace trip
{
    namespace client
    {

        namespace resource_error {

            enum errors
            {
                segment_count = 1, 
                segment_size = 2, 
                segment_duration = 3, 
                segment_chksum = 4,
            };

            namespace detail {

                class category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "trip/resource";
                    }

                    std::string message(int value) const
                    {
                        if (value == segment_count)
                            return "Resource segment count error";
                        if (value == segment_size)
                            return "Resource segment size error";
                        if (value == segment_duration)
                            return "Resource segment duration error";
                        if (value == segment_chksum)
                            return "Resource segment ckeck sum error";
                        return "Resource error";
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::category instance;
                return instance;
            }

            static boost::system::error_category const & trip_resource_category = get_category();

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace error

    } // namespace client
} // namespace trip

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<trip::client::resource_error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

    }
}

#endif // _TRIP_CORE_ERROR_H_
