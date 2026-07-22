#include "log.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace hglv {

LogLevel               Logger::_minLevel    = LogLevel::Debug;
std::mutex             Logger::_mutex;
std::ofstream          Logger::_file;
bool                   Logger::_initialized  = false;
std::filesystem::path  Logger::_folder;

void Logger::Init(const std::filesystem::path &filename,
                  const std::filesystem::path &folder,
                  const std::string &level)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (level == "trace")   _minLevel = LogLevel::Trace;
    else if (level == "debug")   _minLevel = LogLevel::Debug;
    else if (level == "info")    _minLevel = LogLevel::Info;
    else if (level == "warning") _minLevel = LogLevel::Warning;
    else if (level == "error")   _minLevel = LogLevel::Error;
    else                         _minLevel = LogLevel::Debug;

    _folder = folder;
    _initialized = true;

    // Create log folder if it doesn't exist
    std::error_code ec;
    std::filesystem::create_directories(folder, ec);

    // Open log file
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto *tm = std::localtime(&time);
    char timeBuf[64];
    std::strftime(timeBuf, sizeof(timeBuf), "_%Y%m%d_%H%M%S.log", tm);

    auto logPath = folder / (filename.string() + timeBuf);
    _file.open(logPath, std::ios::out | std::ios::app);
    if (!_file.is_open()) {
        std::cerr << "[WARN] Failed to open log file: " << logPath << std::endl;
    } else {
        std::cerr << "[INFO] Log file: " << logPath << std::endl;
    }
}

void Logger::Configure(const std::filesystem::path &setting)
{
    std::ifstream ifs(setting);
    if (!ifs.is_open()) {
        std::cerr << "[WARN] Cannot open log config: " << setting << std::endl;
        return;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        // Simple key=value parsing
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        auto key = line.substr(0, pos);
        auto value = line.substr(pos + 1);

        if (key == "level" || key == "Level") {
            if (value == "trace")        _minLevel = LogLevel::Trace;
            else if (value == "debug")   _minLevel = LogLevel::Debug;
            else if (value == "info")    _minLevel = LogLevel::Info;
            else if (value == "warning") _minLevel = LogLevel::Warning;
            else if (value == "error")   _minLevel = LogLevel::Error;
        }
    }
}

} // namespace hglv
