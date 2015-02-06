// ResourceCache.cpp

#include "trip/client/Common.h"
#include "trip/client/cache/ResourceCache.h"
#include "trip/client/core/Resource.h"

#include <framework/string/Parse.h>

#include <boost/filesystem/operations.hpp>

#include <fstream>

namespace trip
{
    namespace client
    {

        ResourceCache::ResourceCache(
            Resource & resource, 
            WorkQueue & queue)
            : resource_(resource)
            , queue_(queue)
        {
        }

        ResourceCache::~ResourceCache()
        {
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
                if (iter->path().extension() == resource_.meta().file_extension) {
                    boost::uint64_t segid = framework::string::parse<boost::uint64_t>(iter->path().stem());
                    boost::uint64_t id = MAKE_ID(segid, 0, 0);
                    if (!resource_.load_segment(id, iter->path())) {
                        boost::filesystem::remove(iter->path());
                    }
                }
            }
            return true;
        }

    } // namespace client
} // namespace trip
