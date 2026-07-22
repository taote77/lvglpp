#ifndef NAMESPACELESS_LOGGER_H
#define NAMESPACELESS_LOGGER_H

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

#define LOG_DEBUG()  hglv::detail::logStream(hglv::LogLevel::Debug,   __PRETTY_FUNCTION__)
#define LOG_INFO()   hglv::detail::logStream(hglv::LogLevel::Info,    __PRETTY_FUNCTION__)
#define LOG_WARN()   hglv::detail::logStream(hglv::LogLevel::Warning, __PRETTY_FUNCTION__)
#define LOG_ERROR()  hglv::detail::logStream(hglv::LogLevel::Error,   __PRETTY_FUNCTION__)
#define LogDebug     LOG_DEBUG()
#define LogWarn      LOG_WARN()
#define LogError     LOG_ERROR()

#endif // LOGGER_H
