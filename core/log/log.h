#ifndef NAMESPACELESS_LOGGER_H
#define NAMESPACELESS_LOGGER_H

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/current_function.hpp>

namespace hglv{

class Logger
{
public:
    static void Init(const boost::filesystem::path& filename, const boost::filesystem::path& folder,const std::string& level);

    static void Configure(const boost::filesystem::path& setting);
};

}



#ifndef LOG_DEBUG
#define LOG_DEBUG() BOOST_LOG_TRIVIAL(debug)<<BOOST_CURRENT_FUNCTION<<"|"
#endif

#ifndef LOG_INFO
#define LOG_INFO() BOOST_LOG_TRIVIAL(info)<<BOOST_CURRENT_FUNCTION<<"|"
#endif

#ifndef LOG_WARN
#define LOG_WARN()  BOOST_LOG_TRIVIAL(warning)<<BOOST_CURRENT_FUNCTION<<"|"
#endif

#ifndef LOG_ERROR
#define LOG_ERROR() BOOST_LOG_TRIVIAL(error)<<BOOST_CURRENT_FUNCTION<<"|"
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
