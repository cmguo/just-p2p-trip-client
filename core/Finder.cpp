// Finder.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Finder.h"
#include "trip/client/core/Resource.h"

namespace trip
{
    namespace client
    {

        Finder::Finder()
        {
        }

        Finder::~Finder()
        {
        }

        void Finder::find(
            Resource & resource, 
            size_t count, 
            resp_t const & resp)
        {
            requests_[resource.id()] = std::make_pair(count, resp);
            find(resource, count);
        }

        void Finder::cancel(
            Resource & resource)
        {
            requests_.erase(resource.id());
        }
            
        void Finder::found(
            boost::system::error_code const & ec, 
            Resource & resource, 
            std::vector<Url> const & urls)
        {
            std::map<Uuid, std::pair<size_t, resp_t> >::iterator iter = 
                requests_.find(resource.id());
            if (iter != requests_.end()) {
                resp_t resp;
                resp.swap(iter->second.second);
                requests_.erase(iter);
                resp(ec, urls);
            }
        }

    } // namespace client
} // namespace trip
