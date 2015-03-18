// Finder.cpp

#include "trip/client/Common.h"
#include "trip/client/core/Finder.h"
#include "trip/client/core/Resource.h"
#include "trip/client/core/Scheduler.h"

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
            Scheduler & scheduler, 
            size_t count)
        {
            requests_.insert(&scheduler);
            find_more(scheduler.resource(), count);
        }

        void Finder::cancel(
            Scheduler & scheduler)
        {
            requests_.erase(&scheduler);
        }

        void Finder::create_source(
            Scheduler & scheduler)
        {
            std::vector<Url> const & urls = scheduler.resource().urls();
            for (size_t i = 0; i < urls.size(); ++i) {
                Source * s = create(scheduler, urls[i]);
                if (s)
                    scheduler.add_source(s);
            }
        }

        void Finder::found(
            Resource & resource, 
            std::vector<Url> & urls)
        {
            std::set<Scheduler *>::iterator iter = requests_.begin();
            while (iter != requests_.end()) {
                if (&(*iter)->resource() == &resource) {
                    Scheduler & scheduler = **iter;
                    requests_.erase(iter);
                    scheduler.resource().set_urls(urls);
                    for (size_t i = 0; i < urls.size(); ++i) {
                        Source * s = create(scheduler, urls[i]);
                        if (s)
                            scheduler.add_source(s);
                    }
                    break;
                }
            }
        }

    } // namespace client
} // namespace trip
