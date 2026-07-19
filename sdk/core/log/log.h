#ifndef NAMESPACELESS_LOGGER_H
#define NAMESPACELESS_LOGGER_H

#include <boost/log/trivial.hpp>
#include <filesystem>

namespace hglv{

class Logger
{
public:
    static void Init(const std::filesystem::path& filename, const std::filesystem::path& folder,const std::string& level);

    static void Configure(const std::filesystem::path& setting);
};

}



#ifndef LOG_DEBUG
#define LOG_DEBUG() BOOST_LOG_TRIVIAL(debug)<<__PRETTY_FUNCTION__<<"|"
#endif

#ifndef LOG_INFO
#define LOG_INFO() BOOST_LOG_TRIVIAL(info)<<__PRETTY_FUNCTION__<<"|"
#endif

#ifndef LOG_WARN
#define LOG_WARN()  BOOST_LOG_TRIVIAL(warning)<<__PRETTY_FUNCTION__<<"|"
#endif

#ifndef LOG_ERROR
#define LOG_ERROR() BOOST_LOG_TRIVIAL(error)<<__PRETTY_FUNCTION__<<"|"
#endif

#ifndef LogDebug
#define LogDebug LOG_DEBUG()
#endif

#ifndef LogWarn
#define LogWarn LOG_WARN()
#endif

#ifndef LogError
#define LogError LOG_ERROR()
#endif


#endif // LOGGER_H
