// ReportInfo.h

#ifndef _JUST_MAIN_REPORT_INFO_H_
#define _JUST_MAIN_REPORT_INFO_H_

#include <util/serialization/stl/vector.h>

namespace trip
{
    namespace client
    {

        struct ReportParam
        {
            std::string name;
            std::string path;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(name)
                    & SERIALIZATION_NVP(path);
            }
        };

        struct ReportInfo
        {
            std::string name;
            Url url;
            std::vector<ReportParam> params;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(name)
                    & SERIALIZATION_NVP(url)
                    & SERIALIZATION_NVP(params);
            }

            friend bool operator==(
                ReportInfo const & info, 
                std::string const & name)
            {
                return info.name == name;
            }
        };

        struct ReportSet
        {
            Url base_url;
            std::vector<ReportInfo> infos;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(base_url)
                    & SERIALIZATION_NVP(infos);
            }
        };

    } // namespace client
} // namespace trip

#endif // _JUST_MAIN_REPORT_INFO_H_
