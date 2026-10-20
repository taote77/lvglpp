#ifndef WSGW_UTIL_H
#define WSGW_UTIL_H

#include <cstddef>
#include <string>
#include <string_view>

namespace wsgw {

// Standard Base64 encoding (with padding).
std::string base64Encode(const void *data, std::size_t size);

inline std::string base64Encode(const std::string &data)
{
    return base64Encode(data.data(), data.size());
}

// Computes the Sec-WebSocket-Accept value for a client key.
std::string webSocketAcceptKey(std::string_view clientKey);

} // namespace wsgw

#endif // WSGW_UTIL_H
