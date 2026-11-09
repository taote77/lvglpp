#ifndef LVGLPP_STBIMAGELOCK_H
#define LVGLPP_STBIMAGELOCK_H

#include <mutex>

namespace lvglpp::tools {

/**
 * stb_image keeps mutable process-global state (stbi__g_failure_reason and
 * decoder contexts). All stb calls from the SDK are serialized through this
 * mutex so the prefetch worker cannot race Image base64 decoding.
 */
inline std::mutex &stbImageMutex()
{
    static std::mutex mutex;
    return mutex;
}

} // namespace lvglpp::tools

#endif // LVGLPP_STBIMAGELOCK_H
