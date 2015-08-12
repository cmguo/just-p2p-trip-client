// Format.h

#ifndef _TRIP_CLIENT_UTILS_FORMAT_H_
#define _TRIP_CLIENT_UTILS_FORMAT_H_

#include <boost/filesystem/path.hpp>

namespace boost
{
    namespace filesystem
    {

        inline std::string to_string(
            boost::filesystem::path const & path)
        {
            return path.string();
        }

        inline bool from_string(
            std::string const & str, 
            boost::filesystem::path & path)
        {
            path = str;
            return true;
        }

    } // namespace filesystem
} // namespace boost

#endif // _TRIP_CLIENT_UTILS_FORMAT_H_

