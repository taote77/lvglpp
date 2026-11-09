#ifndef LVGLPP_IMAGEMANAGER_H
#define LVGLPP_IMAGEMANAGER_H

#include "ImageResource.h"

#include <cstddef>
#include <string>
#include <vector>

namespace lvglpp::tools {

/**
 * Decoded-image cache with LRU eviction and optional background prefetch.
 *
 * Keys are logical asset paths ("image/40x40/icon.png"). File-backed images
 * use loadFile() and an absolute-path key. All methods are thread-safe; no
 * LVGL API is called from this class, so prefetching may run off the UI
 * thread. Widgets must keep the returned ImageHandle alive for as long as
 * LVGL references its descriptor.
 */
class ImageManager
{
public:
    using Handle = ImageHandle;

    /// Load an application asset (mounted through AssetManager).
    static Handle load(const std::string &assetPath);

    /// Load a decoded image from the real filesystem.
    static Handle loadFile(const std::string &fsPath);

    /// Warm the cache on a background worker (safe to call from any thread).
    static void prefetch(const std::string &assetPath);
    static void prefetch(const std::vector<std::string> &assetPaths);

    /// Block until the prefetch queue has been processed.
    static void waitForPrefetch();

    /// Drop one cached entry (e.g. after a directory-mode hot reload).
    static void invalidate(const std::string &assetPath);
    static void invalidateFile(const std::string &fsPath);

    /// Configure the LRU (defaults: 32 entries / 64 MB).
    static void setCacheLimits(std::size_t maxEntries, std::size_t maxBytes);

    /// Drop cache-owned references. Handles still in use remain valid.
    static void clearCache();

    static std::size_t cacheSize();
    static std::size_t cacheBytes();

    /// key is an asset path or a "file:<abs-path>" key.
    static bool isCached(const std::string &key);

    /// Stop the prefetch worker and clear the cache (idempotent).
    static void shutdown();
};

} // namespace lvglpp::tools

#endif // LVGLPP_IMAGEMANAGER_H
