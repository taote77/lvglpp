#ifndef WSGW_JSON_H
#define WSGW_JSON_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace wsgw {

/**
 * Minimal JSON value used by the WebSocket gateway.
 *
 * Supports the subset of the JSON model needed by ws_gw: null, bool,
 * integer, double, string, array and object.  It deliberately has no
 * external dependencies so that ws_gw stays lightweight.
 */
class Json
{
public:
    enum class Type { Null, Bool, Int, Double, String, Array, Object };

    Json();
    Json(std::nullptr_t);
    Json(bool value);
    Json(int64_t value);
    Json(double value);
    Json(const char *value);
    Json(std::string value);
    Json(std::string_view value);

    template <typename T,
              typename = std::enable_if_t<std::is_integral_v<T> &&
                                          !std::is_same_v<std::decay_t<T>, bool> &&
                                          !std::is_same_v<std::decay_t<T>, int64_t>>>
    Json(T value) : Json(static_cast<int64_t>(value))
    {}

    Json(const Json &other);
    Json(Json &&other) noexcept;
    Json &operator=(const Json &other);
    Json &operator=(Json &&other) noexcept;
    ~Json();

    static Json makeObject();
    static Json makeArray();

    Type type() const;

    bool is_null() const;
    bool is_bool() const;
    bool is_int() const;
    bool is_double() const;
    bool is_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;

    bool              as_bool() const;
    int64_t           as_int64() const;
    double            as_double() const;
    const std::string &as_string() const;
    std::string       &as_string();
    Json              &as_object();
    const Json        &as_object() const;
    Json              &as_array();
    const Json        &as_array() const;

    // Object member access. Non-const access inserts a null member when the
    // key is missing (and turns a null value into an object).
    Json       &operator[](const std::string &key);
    Json       &operator[](const char *key);
    const Json &operator[](const std::string &key) const;
    const Json &operator[](const char *key) const;

    bool        contains(const std::string &key) const;
    bool        empty() const;
    std::size_t size() const;

    Json &push_back(Json value);

    std::string serialize() const;
    static Json parse(std::string_view text);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    static void serializeInto(const Json &value, std::string &out);
    static void escapeString(std::string_view value, std::string &out);
};

std::ostream &operator<<(std::ostream &os, const Json &value);

} // namespace wsgw

#endif // WSGW_JSON_H
