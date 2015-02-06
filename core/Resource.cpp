// Resource.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        Resource::Resource()
        {
        }

        Resource::~Resource()
        {
        }

        bool Resource::load(
            boost::filesystem::path const & path, 
            boost::system::error_code & ec)
        {
            return false;
        }

        void Resource::set_id(
            Uuid const & id)
        {
            id_ = id;
            raise(meta_changed);
        }

        void Resource::set_meta(
            ResourceMeta const & meta)
        {
            meta_ = meta;
            raise(meta_changed);
        }

        void Resource::set_urls(
            std::vector<Url> const & urls)
        {
            urls_ = urls;
            raise(meta_changed);
        }

    } // namespace client
} // namespace trip
