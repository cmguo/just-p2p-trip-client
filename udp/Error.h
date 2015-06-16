// Error.h

#ifndef _TRIP_UDP_ERROR_H_
#define _TRIP_UDP_ERROR_H_

namespace trip
{
    namespace client
    {

        namespace udp_error {

            enum errors
            {
                chksum_error = 1, 
            };

            namespace detail {

                class category
                    : public boost::system::error_category
                {
                public:
                    const char* name() const
                    {
                        return "trip/udp";
                    }

                    std::string message(int value) const
                    {
                        if (value == chksum_error)
                            return "Udp check sum error";
                        return "Udp error";
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::category instance;
                return instance;
            }

            static boost::system::error_category const & trip_udp_category = get_category();

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
        struct is_error_code_enum<trip::client::udp_error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

    }
}

#endif // _TRIP_UDP_ERROR_H_
