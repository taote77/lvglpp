#ifndef LVGLPP_LOG_ESP_H
#define LVGLPP_LOG_ESP_H

/*
 * ESP-IDF logging backend for the hglv::Logger-compatible macros in log.h.
 *
 * Keeps the existing `LOG_DEBUG() << "x" << value` call sites intact while
 * avoiding <filesystem>, <fstream>, <iostream> and std::thread. Messages are
 * buffered into a small fixed-size line and emitted with ESP_LOGx.
 *
 * Define LVGLPP_LOG_DISABLE to compile every log statement into a no-op.
 */

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

#include <esp_log.h>

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
    /// File logging is not available on ESP-IDF; kept for source compatibility.
    template <class PathA, class PathB>
    static void Init(const PathA &, const PathB &, const std::string &)
    {}

    template <class Path>
    static void Configure(const Path &)
    {}

    static LogLevel minLevel()
    {
        return LogLevel::Debug;
    }

    static void setMinLevel(LogLevel)
    {}
};

namespace detail {

inline constexpr const char *kLogTag = "lvglpp";

#ifdef LVGLPP_LOG_DISABLE

class NullLogStream
{
public:
    template <class T>
    NullLogStream &operator<<(const T &)
    {
        return *this;
    }
};

#else

class EspLogStream
{
public:
    explicit EspLogStream(esp_log_level_t level) : level_(level)
    {}

    ~EspLogStream()
    {
        if (length_ > 0)
        {
            esp_log_write(level_, kLogTag, "%s", buffer_);
        }
    }

    EspLogStream(const EspLogStream &)            = delete;
    EspLogStream &operator=(const EspLogStream &) = delete;

    EspLogStream &operator<<(const char *value)
    {
        append(value != nullptr ? value : "(null)");
        return *this;
    }

    EspLogStream &operator<<(const std::string &value)
    {
        append(value.data(), value.size());
        return *this;
    }

    EspLogStream &operator<<(std::string_view value)
    {
        append(value.data(), value.size());
        return *this;
    }

    EspLogStream &operator<<(char value)
    {
        append(&value, 1);
        return *this;
    }

    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    EspLogStream &operator<<(T value)
    {
        char number[32];
        if constexpr (std::is_signed_v<T>)
        {
            std::snprintf(number, sizeof(number), "%lld", static_cast<long long>(value));
        } else
        {
            std::snprintf(number, sizeof(number), "%llu", static_cast<unsigned long long>(value));
        }
        append(number);
        return *this;
    }

    template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    EspLogStream &operator<<(T value)
    {
        char number[32];
        std::snprintf(number, sizeof(number), "%g", static_cast<double>(value));
        append(number);
        return *this;
    }

    template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
    EspLogStream &operator<<(T value)
    {
        return *this << static_cast<long long>(value);
    }

    EspLogStream &operator<<(const void *value)
    {
        char address[32];
        std::snprintf(address, sizeof(address), "%p", value);
        append(address);
        return *this;
    }

private:
    void append(const char *text)
    {
        append(text, text != nullptr ? std::strlen(text) : 0);
    }

    void append(const char *text, std::size_t size)
    {
        if (text == nullptr || size == 0 || length_ + 1 >= sizeof(buffer_))
        {
            return;
        }
        const std::size_t room = sizeof(buffer_) - 1 - length_;
        if (size > room)
        {
            size = room;
        }
        std::memcpy(buffer_ + length_, text, size);
        length_ += size;
        buffer_[length_] = '\0';
    }

    char            buffer_[256]{};
    std::size_t     length_ = 0;
    esp_log_level_t level_;
};

#endif // LVGLPP_LOG_DISABLE

} // namespace detail
} // namespace hglv

#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LogDebug
#undef LogWarn
#undef LogError

#ifdef LVGLPP_LOG_DISABLE
#define LOG_DEBUG() hglv::detail::NullLogStream()
#define LOG_INFO()  hglv::detail::NullLogStream()
#define LOG_WARN()  hglv::detail::NullLogStream()
#define LOG_ERROR() hglv::detail::NullLogStream()
#else
#define LOG_DEBUG() hglv::detail::EspLogStream(ESP_LOG_DEBUG)
#define LOG_INFO()  hglv::detail::EspLogStream(ESP_LOG_INFO)
#define LOG_WARN()  hglv::detail::EspLogStream(ESP_LOG_WARN)
#define LOG_ERROR() hglv::detail::EspLogStream(ESP_LOG_ERROR)
#endif

#define LogDebug LOG_DEBUG()
#define LogWarn  LOG_WARN()
#define LogError LOG_ERROR()

#endif // LVGLPP_LOG_ESP_H
