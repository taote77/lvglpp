#ifndef NAMESPACELESS_LOGGER_H
#define NAMESPACELESS_LOGGER_H

#ifdef ESP_PLATFORM

// ESP-IDF: ESP_LOGx backend, no std::filesystem / fstream / iostream.
#include "log_esp.h"

#else // ESP_PLATFORM

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

namespace hglv {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error
};

class Logger
{
public:
    static void Init(const std::filesystem::path &filename,
                     const std::filesystem::path &folder,
                     const std::string &level);

    static void Configure(const std::filesystem::path &setting);

    static LogLevel minLevel() { return _minLevel; }
    static void setMinLevel(LogLevel level) { _minLevel = level; }

    static void log(LogLevel level, const std::string &function, const std::string &message);

private:
    static LogLevel               _minLevel;
    static std::mutex             _mutex;
    static std::ofstream          _file;
    static bool                   _initialized;
    static std::filesystem::path  _folder;
};

inline void Logger::log(LogLevel level, const std::string &function,
                         const std::string &message)
{
    if (level < _minLevel) return;

    const char *levelStr = "UNKNOWN";
    switch (level) {
        case LogLevel::Trace:   levelStr = "TRACE"; break;
        case LogLevel::Debug:   levelStr = "DEBUG"; break;
        case LogLevel::Info:    levelStr = "INFO";  break;
        case LogLevel::Warning: levelStr = "WARN";  break;
        case LogLevel::Error:   levelStr = "ERROR"; break;
    }

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

    std::ostringstream oss;
    oss << "[" << timeBuf << "] [" << levelStr << "] "
        << function << " | " << message << std::endl;

    std::lock_guard<std::mutex> lock(_mutex);
    std::cerr << oss.str();
    if (_file.is_open()) {
        _file << oss.str();
        _file.flush();
    }
}

} // namespace hglv

// ---- Log macros (backward compatible with existing code) ----

namespace hglv::detail {
inline std::ostream &logStream(hglv::LogLevel level, const char *function) {
    if (level < Logger::minLevel()) {
        static std::ostream nullStream(nullptr);
        return nullStream;
    }
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

    const char *levelStr = "UNKNOWN";
    switch (level) {
        case LogLevel::Debug:   levelStr = "DEBUG"; break;
        case LogLevel::Info:    levelStr = "INFO";  break;
        case LogLevel::Warning: levelStr = "WARN";  break;
        case LogLevel::Error:   levelStr = "ERROR"; break;
        default: break;
    }

    std::cerr << "[" << timeBuf << "] [" << levelStr << "] " << function << "|";
    return std::cerr;
}
} // namespace hglv::detail

#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LogDebug
#undef LogWarn
#undef LogError

#if defined(_MSC_VER)
#define LVGLPP_LOG_FUNCTION __FUNCSIG__
#else
#define LVGLPP_LOG_FUNCTION __PRETTY_FUNCTION__
#endif

#define LOG_DEBUG()  hglv::detail::logStream(hglv::LogLevel::Debug,   LVGLPP_LOG_FUNCTION)
#define LOG_INFO()   hglv::detail::logStream(hglv::LogLevel::Info,    LVGLPP_LOG_FUNCTION)
#define LOG_WARN()   hglv::detail::logStream(hglv::LogLevel::Warning, LVGLPP_LOG_FUNCTION)
#define LOG_ERROR()  hglv::detail::logStream(hglv::LogLevel::Error,   LVGLPP_LOG_FUNCTION)
#define LogDebug     LOG_DEBUG()
#define LogWarn      LOG_WARN()
#define LogError     LOG_ERROR()

#endif // ESP_PLATFORM

#endif // NAMESPACELESS_LOGGER_H
