// ResourceCache.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/ResourceCache.h"
#include "trip/client/cache/CacheManager.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Serialize.h"
#include "trip/client/proto/MessageResource.h"

#include <util/archive/XmlIArchive.h>
#include <util/archive/XmlOArchive.h>

#include <framework/string/Base16.h>
#include <framework/system/BytesOrder.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

namespace trip
{
    namespace client
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.ResourceCache", framework::logger::Debug);

        ResourceCache::ResourceCache(
            Resource & resource, 
            boost::filesystem::path const & directory)
            : resource_(resource)
            , is_external_(false)
        {
            boost::system::error_code ec;
            directory_ = directory / resource_.id().to_string();
            boost::filesystem::path external(directory_ / "external");
            if (boost::filesystem::exists(external, ec)) {
                std::ifstream ifs(external.string().c_str());
                std::string line;
                if (std::getline(ifs, line)) {
                    directory_ = line;
                    is_external_ = true;
                }
            }
            if (!boost::filesystem::exists(directory_, ec)) {
                if (!boost::filesystem::create_directory(directory_, ec)) {
                    if (is_external_) {
                        directory_ = directory / resource_.id().to_string();
                        is_external_ = false;
                    }
                }
            }
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
            boost::uint64_t id2 = framework::system::BytesOrder::host_to_big_endian(id.id);
            std::string sid((char const *)&id2, sizeof(id2));
            return framework::string::Base16::encode(sid);
        }

        bool ResourceCache::save_status()
        {
            if (!resource_.meta_dirty())
                return false;
            ResourceInfo info;
            info.id = resource_.id();
            info.meta = *resource_.meta();
            info.segments = std::vector<SegmentMeta>();
            resource_.get_segments(info.segments.get());
            std::ofstream ofs((directory_ / "meta.xml").string().c_str(), std::ios::binary);
            util::archive::XmlOArchive<> oa(ofs);
            oa << info;
            if (!oa) return false;
            return true;
        }

        // Run in work thread
        bool ResourceCache::load_status(
            boost::dynamic_bitset<boost::uint32_t> & map)
        {
            ResourceInfo info;
            std::ifstream is((directory_ / "meta.xml").string().c_str(), std::ios::binary);
            util::archive::XmlIArchive<> ia(is);
            ia >> info;
            if (!ia) return false;

            std::vector<SegmentMeta> const & segments(info.segments.get());
            resource_.set_segments(segments);
            resource_.set_meta(info.meta);
            resource_.meta_dirty(); // clear dirty mark

            boost::filesystem::directory_iterator iter(directory_);
            boost::filesystem::directory_iterator end;
            boost::system::error_code ec;
            for (; iter != end; ++iter) {
                if (iter->path().extension() == resource_.meta()->file_extension) {
                    boost::uint64_t segid = strid(iter->path().stem().string());
                    if (segments[segid].bytesize != boost::filesystem::file_size(iter->path())) {
                        LOG_WARN("[load_status] size not match (to be deteled), segment=" << segid);
                        boost::filesystem::remove(iter->path(), ec);
                        continue;
                    }
                    if (map.size() <= segid)
                        map.resize(segid + 1);
                    map[segid] = true;
                }
            }
            return true;
        }

        Block const * ResourceCache::map_block(
            DataId bid)
        {
            //LOG_INFO("[map_block] rid=" << resource_.id() << ", id=" << bid);
            return resource_.map_block(bid, seg_path(bid));
        }

        // Run in work thread
        bool ResourceCache::save_segment(
            DataId sid, 
            Segment2 const & segment)
        {
            LOG_DEBUG("[save_segment] rid=" << resource_.id() << ", segment=" << sid.segment);
            if (segment.meta == NULL || segment.seg == NULL) {
                return false;
            }
            if (segment.seg->cache_md5sum() != segment.meta->md5sum) {
                LOG_WARN("[save_segment] md5sum not match, segment=" << sid.segment);
                return false;
            }
            boost::system::error_code ec;
            bool ret = segment.seg->save(seg_path(sid), ec);
            if (!ret)
                LOG_WARN("[save_segment] failed, ec=" << ec.message());
            return ret;
        }

        // Run in work thread
        bool ResourceCache::load_segment(
            DataId sid, 
            SegmentMeta const & segment)
        {
            LOG_DEBUG("[load_segment] rid=" << resource_.id() << " segment=" << sid.segment);
            boost::filesystem::path path = seg_path(sid);
            Md5Sum md5 = Segment::file_md5sum(path);
            if (md5 != segment.md5sum) {
                LOG_WARN("[load_segment] md5sum not match (to be deteled), segment=" << sid.segment);
                boost::system::error_code ec;
                boost::filesystem::remove(path, ec);
                return false;
            }
            return true;
        }

        bool ResourceCache::free_segment(
            DataId sid)
        {
            LOG_DEBUG("[free_segment] rid=" << resource_.id() << " segment=" << sid.segment);
            boost::filesystem::path path = seg_path(sid);
            boost::system::error_code ec;
            boost::filesystem::remove(path, ec);
            return !ec;
        }

        boost::filesystem::path ResourceCache::seg_path(
            DataId sid) const
        {
            std::string segname = idstr(sid.segment);
            segname.append(resource_.meta()->file_extension);
            return (directory_ / segname);
        }

    } // namespace client
} // namespace trip
