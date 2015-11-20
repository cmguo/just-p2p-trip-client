// UrlHelper.cpp

#include "trip/client/Common.h"
#include "trip/client/utils/UrlHelper.h"

#include <framework/string/Base16.h>
#include <framework/string/Base64.h>
#include <framework/string/Slice.h>
#include <framework/string/Des.h>
#include <framework/system/ErrorCode.h>

#include <fstream>
#include <iterator>

namespace trip
{
    namespace client
    {

        static bool decode_base16(
            std::string const & input, 
            std::string & output, 
            boost::system::error_code & ec)
        {
            std::string result = framework::string::Base16::decode(input);
            if (result == "E1" || result == "E2") {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            }
            output.swap(result);
            return true;
        }

        static bool decode_base64(
            std::string const & input, 
            std::string & output, 
            boost::system::error_code & ec)
        {
            std::string result = framework::string::Base64::decode(input);
            if (result.empty()) {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            }
            output.swap(result);
            return true;
        }

        static char const * const gsKey[] =
        {
            "\x70\x70\x6C\x69\x76\x65\x6F\x6B", // 第一个不会用到
            "\x15\xB9\xFD\xAE\xDA\x40\xF8\x6B\xF7\x1C\x73\x29\x25\x16\x92\x4A\x29\x4F\xC8\xBA\x31\xB6\xE9\xEA",
            "\x29\x02\x8A\x76\x98\xEF\x4C\x6D\x3D\x25\x2F\x02\xF4\xF7\x9D\x58\x15\x38\x9D\xF1\x85\x25\xD3\x26",
            "\xD0\x46\xE6\xB6\xA4\xA8\x5E\xB6\xC4\x4C\x73\x37\x2A\x0D\x5D\xF1\xAE\x76\x40\x51\x73\xB3\xD5\xEC",
            "\x43\x52\x29\xC8\xF7\x98\x31\x13\x19\x23\xF1\x8C\x5D\xE3\x2F\x25\x3E\x2A\xF2\xAD\x34\x8C\x46\x15",
            "\x9B\x29\x15\xA7\x2F\x83\x29\xA2\xFE\x6B\x68\x1C\x8A\xAE\x1F\x97\xAB\xA8\xD9\xD5\x85\x76\xAB\x20",
            "\xB3\xB0\xCD\x83\x0D\x92\xCB\x37\x20\xA1\x3E\xF4\xD9\x3B\x1A\x13\x3D\xA4\x49\x76\x67\xF7\x51\x91",
            "\xAD\x32\x7A\xFB\x5E\x19\xD0\x23\x15\x0E\x38\x2F\x6D\x3B\x3E\xB5\xB6\x31\x91\x20\x64\x9D\x31\xF8",
            "\xC4\x2F\x31\xB0\x08\xBF\x25\x70\x67\xAB\xF1\x15\xE0\x34\x6E\x29\x23\x13\xC7\x46\xB3\x58\x1F\xB0",
            "\x52\x9B\x75\xBA\xE0\xCE\x20\x38\x46\x67\x04\xA8\x6D\x98\x5E\x1C\x25\x57\x23\x0D\xDF\x31\x1A\xBC",
            "\x8A\x52\x9D\x5D\xCE\x91\xFE\xE3\x9E\x9E\xE9\x54\x5D\xF4\x2C\x3D\x9D\xEC\x2F\x76\x7C\x89\xCE\xAB"
        };

        static bool decode_3des(
            std::string const & input, 
            std::string & output, 
            boost::system::error_code & ec)
        {
            boost::uint32_t key_index = 0;
            std::string::size_type colon = input.find('|');
            if (colon == std::string::npos) {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            }
            ec = framework::string::parse2(input.substr(0, colon), key_index);
            if (ec) {
                return false;
            }
            if (key_index >= sizeof(gsKey) / sizeof(gsKey[0]) || key_index == 0) {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            }
            ++colon;
            std::string result;
            result.resize(input.size() - colon);
            if(!framework::string::Des::pptv_3_des_d(input.substr(colon).c_str(), input.size() - colon, gsKey[key_index], 24, &result[0], result.size())) {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            }
            output.swap(result);
            return true;
        }

        static bool decode_file(
            std::string const & input, 
            std::string & output, 
            boost::system::error_code & ec)
        {
            std::ifstream ifs;
            ifs.open(input.c_str(), std::ios::binary);
            if (!ifs) {
                ec = framework::system::last_system_error();
                return false;
            }
            ifs >> std::noskipws;
            std::istream_iterator<char> beg(ifs), end;
            std::copy(beg, end, std::inserter(output, output.end()));
            return true;
        }

        static struct
        {
            std::string name;
            bool (*decoder)(
                std::string const &, 
                std::string &, 
                boost::system::error_code &);
        } decoders[] = {
            {"base16|", decode_base16}, 
            {"base64|", decode_base64}, 
            {"3des|", decode_3des}, 
            {"file|", decode_file}, 
            {"base16/", decode_base16}, 
            {"base64/", decode_base64}, 
            {"3des/", decode_3des}, 
            {"file/", decode_file}, 
        };

        static bool decode(
            std::string const & input, 
            std::string & output, 
            boost::system::error_code & ec)
        {
            for (size_t i = 0; i < sizeof(decoders) / sizeof(decoders[0]); ++i) {
                if (input.compare(0, decoders[i].name.size(), decoders[i].name) == 0) {
                    return decoders[i].decoder(input.substr(decoders[i].name.size()), output, ec);
                }
            }
            output = input;
            return true;
        }

        bool decode_url(
            framework::string::Url & url, 
            boost::system::error_code & ec)
        {
            if (!url.is_valid()) {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            }
            std::string output;
            if (decode(url.path().substr(1), output, ec)) {
                framework::string::Url ur12("http:///" + output);
                url.path(ur12.path());
                for (framework::string::Url::param_iterator iter = ur12.param_begin(); iter != ur12.param_end(); ++iter) {
                    url.param(iter->key(), iter->value());
                }
            }
            return true;
        }

        void apply_config(
            framework::configure::Config & config, 
            framework::string::Url const & url, 
            std::string const & prefix)
        {
            framework::string::Url::param_const_iterator iter = url.param_begin();
            for (; iter != url.param_end(); ++iter) {
                std::string key = iter->key();
                if (key.compare(0, prefix.size(), prefix) == 0) {
                    std::string::size_type pos_dot = key.rfind('.');
                    if (pos_dot == std::string::npos || pos_dot <= prefix.size())
                        continue;
                    config.set_force(
                        key.substr(prefix.size(), pos_dot - prefix.size()), 
                        key.substr(pos_dot + 1), 
                        iter->value());
                }
            }
        }

        bool decode_param(
            framework::string::Url & url, 
            std::string const & key, 
            boost::system::error_code & ec)
        {
            std::string result;
            if (!decode(url.param(key), result, ec)) {
                return false;
            }
            url.param(key, result);
            return true;
        }

        void apply_params(
            framework::string::Url & dst, 
            framework::string::Url const & src)
        {
            for (framework::string::Url::param_const_iterator iter = src.param_begin(); iter != src.param_end(); ++iter) {
                if (dst.param(iter->key()).empty())
                    dst.param(iter->key(), iter->value());
            }
        }

    } // namespace client
} // namespace trip
