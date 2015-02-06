// IClient.cpp

#include "trip/client/Common.h"

#include "trip/client/cache/CacheManager.h"
#include "trip/client/udp/UdpManager.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("trip.client.Main", framework::logger::Debug);

namespace trip
{
    namespace client
    {
    
        class Client
            : public util::daemon::Daemon
        {
        public:
            Client()
                : util::daemon::Daemon("trip.conf")
            {
    
                char const * default_argv[] = {
                    "++framework.logger.Stream.0.file=$LOG/trip.log", 
                    "++framework.logger.Stream.0.append=true", 
                    "++framework.logger.Stream.0.roll=true", 
                    "++framework.logger.Stream.0.level=5", 
                    "++framework.logger.Stream.0.size=102400", 
                };
                parse_cmdline(sizeof(default_argv) / sizeof(default_argv[0]), default_argv);
    
                framework::logger::load_config(config());
    
                util::daemon::use_module<CacheManager>(*this);
                util::daemon::use_module<UdpManager>(*this);

                LOG_INFO("Client ready.");
            }
        };

        util::daemon::Daemon & global_daemon()
        {
            static Client the_client;
            return the_client;
        }

    } // namespace client
} // namespace trip
