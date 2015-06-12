// ResourceCache.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/CacheManager.h"
#include "trip/client/core/Resource.h"

#include <framework/string/Base16.h>
#include <framework/system/BytesOrder.h>

#include <boost/filesystem/operations.hpp>

#include <fstream>

namespace trip
{
    namespace client
    {

        ResourceCache::ResourceCache(
            CacheManager & cmgr, 
            Resource & resource)
            : cmgr_(cmgr)
            , resource_(resource)
        {
        }

        ResourceCache::~ResourceCache()
        {
        }

        static boost::uint64_t strid(
            std::string const & str)
        {
            std::string sid = framework::string::Base16::decode(str);
            boost::uint64_t id;
            memcpy(&id, sid.c_str(), sizeof(id));
            return framework::system::BytesOrder::big_endian_to_host(id);
        }

        static std::string idstr(
            DataId id)
        {
            id = framework::system::BytesOrder::host_to_big_endian(id);
            std::string sid((char const *)&id, sizeof(id));
            return framework::string::Base16::encode(sid);
        }

        bool ResourceCache::load_status(
            boost::filesystem::path const & directory)
        {
            directory_ = directory;
            boost::filesystem::path external(directory_ / "external");
            if (boost::filesystem::exists(external)) {
                std::ifstream ifs(external.file_string().c_str());
                std::string line;
                if (std::getline(ifs, line)) {
                    directory_ = line;
                }
            }
            if (!boost::filesystem::exists(directory_))
                return false;
            boost::filesystem::directory_iterator iter(directory_);
            boost::filesystem::directory_iterator end;
            for (; iter != end; ++iter) {
                if (iter->path().extension() == resource_.meta()->file_extension) {
                    boost::uint64_t segid = strid(iter->path().stem());
                    DataId id(segid, 0, 0);
                    if (!resource_.load_segment(id, iter->path())) {
                        boost::filesystem::remove(iter->path());
                    }
                }
            }
            return true;
        }

        void ResourceCache::on_event(
            util::event::Event const & event)
        {
            if (event == resource_.segment_full) {
            } else if (event == resource_.data_miss) {
                DataEvent const & e(resource_.data_miss);
                std::string segname = idstr(e.id.segment);
                segname.append(1, '.');
                segname.append(resource_.meta()->file_extension);
                boost::filesystem::path segpath(directory_ / segname);
                cmgr_.alloc_cache(resource_, resource_.data_miss.id, segpath);
            }
        }

    } // namespace client
} // namespace trip
