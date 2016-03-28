// ReportInfo.h

#ifndef _JUST_MAIN_REPORT_INFO_H_
#define _JUST_MAIN_REPORT_INFO_H_

#include <util/serialization/stl/vector.h>
#include <util/serialization/Optional.h>

#include <framework/string/Format.h>
#include <framework/string/Parse.h>

namespace trip
{
    namespace client
    {

        struct ReportValueProc
        {
            typedef ReportValueProc * (*creater)();
            virtual ~ReportValueProc() {};
            virtual void calc(std::string &) = 0;
        };
        
        struct ReportValueProc_incremmental : ReportValueProc
        {
            static ReportValueProc * create() {
                return new ReportValueProc_incremmental;
            }
            boost::uint64_t last;
            ReportValueProc_incremmental() { last = 0; }
            virtual void calc(std::string & value) {
                using namespace framework::string;
                boost::uint64_t curt = parse<boost::uint64_t>(value);
                value = format(curt - last);
                last = curt;
            }
        };

        struct ReportParam
        {
            std::string name;
            std::string value;
            boost::optional<std::string> procs;
            std::vector<ReportValueProc *> vprocs;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(name)
                    & SERIALIZATION_NVP(value)
                    & SERIALIZATION_NVP(procs);
            }
        };

        struct ReportInfo
        {
            std::string name;
            std::string event;
            std::vector<std::string> conds;
            Url url;
            std::vector<ReportParam> params;

            template <typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & SERIALIZATION_NVP(name)
                    & SERIALIZATION_NVP(event)
                    & SERIALIZATION_NVP(conds)
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
