// Resource.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/ResourceEvent.hpp"

#include <util/event/EventEx.hpp>

#include <algorithm>

namespace trip
{
    namespace client
    {

        Resource::Resource()
            : meta_(NULL)
        {
        }

        Resource::~Resource()
        {
            if (meta_) {
                delete meta_;
                meta_ = NULL;
            }
        }

        /*
        bool Resource::load(
            boost::filesystem::path const & path, 
            boost::system::error_code & ec)
        {
            return false;
        }
        */

        void Resource::set_id(
            Uuid const & id)
        {
            id_ = id;
        }

        void Resource::set_meta(
            ResourceMeta const & meta)
        {
            if (meta_) {
                delete meta_;
            }
            meta_ = new ResourceMeta(meta);
            ResourceData::set_meta(meta);
        }

    } // namespace client
} // namespace trip
