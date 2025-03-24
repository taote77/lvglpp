#include "log.h"

#include <fstream>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/utility/setup/from_settings.hpp>
#include <boost/dll.hpp>


namespace hglv{


void Logger::Init(const boost::filesystem::path& filename, const boost::filesystem::path& folder,const std::string& level)
{
    boost::log::add_common_attributes();
    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
    boost::log::register_simple_filter_factory<boost::log::trivial::severity_level, char>("Severity");

    boost::log::settings settings;
    settings["Core"]["Filter"] = "%Severity% >= "+level;
    settings["Core"]["DisableLogging"] = false;


    settings["Sinks.Console"]["Destination"] = "Console";
    settings["Sinks.Console"]["Format"] = "[%TimeStamp%] [%Severity%] %Message% [%ThreadID%]";
    settings["Sinks.Console"]["Filter"] = "%Severity% >= trace";
    settings["Sinks.Console"]["AutoFlush"] = true;


    settings["Sinks.File"]["Destination"] = "TextFile";
    settings["Sinks.File"]["Format"] = "[%TimeStamp%] [%Severity%] %Message% [%ThreadID%]";
    settings["Sinks.File"]["FileName"] = (folder/filename).string()+"_%Y%m%d_%H%M%S_%3N.log";
    settings["Sinks.File"]["Target"] = folder.string();
    settings["Sinks.File"]["Filter"] = "%Severity% >= trace";
    settings["Sinks.File"]["AutoFlush"] = true;
    settings["Sinks.File"]["RotationSize"] = 10 * 1024 * 1024; // 10 MiB
    settings["Sinks.File"]["ScanForFiles"] = "Matching";
    settings["Sinks.File"]["RotationTimePoint"] = "00:00:00";
    boost::log::init_from_settings(settings);
}

void Logger::Configure(const boost::filesystem::path &setting)
{
    try
    {
        std::ifstream ifs(setting.string());
        boost::log::init_from_stream(ifs);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR()<<e.what();
    }
}



}
