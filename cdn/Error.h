// Error.h

#ifndef _TRIP_CDN_ERROR_H_
#define _TRIP_CDN_ERROR_H_

namespace trip
{
    namespace client
    {

        namespace cdn_error {

            enum errors
            {
                xml_format_error = 1, 
            };

            namespace detail {

                class category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "trip/cdn";
                    }

                    std::string message(int value) const
                    {
                        if (value == xml_format_error)
                            return "Cdn xml format error";
                        return "Cdn error";
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::category instance;
                return instance;
            }

            static boost::system::error_category const & trip_cdn_category = get_category();

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
        struct is_error_code_enum<trip::client::cdn_error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

    }
}

#endif // _TRIP_CDN_ERROR_H_
