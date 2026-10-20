#ifndef LVGLPP_IMAGEMANAGERIMPL_H
#define LVGLPP_IMAGEMANAGERIMPL_H

#include "ImageResource.h"

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvglpp::tools {

class ImageManagerImpl
{
public:
    static ImageManagerImpl &getInstance();

    ImageManagerImpl(const ImageManagerImpl &)            = delete;
    ImageManagerImpl &operator=(const ImageManagerImpl &) = delete;

    ImageHandle load(const std::string &assetPath);
    ImageHandle loadFile(const std::string &fsPath);

    void prefetch(const std::vector<std::string> &assetPaths);
    void waitForPrefetch();

    void        invalidate(const std::string &key);
    void        setCacheLimits(std::size_t maxEntries, std::size_t maxBytes);
    void        clearCache();
    std::size_t cacheSize() const;
    std::size_t cacheBytes() const;
    bool        isCached(const std::string &key) const;
    void        shutdown();

    static std::string assetKey(const std::string &assetPath);
    static std::string fileKey(const std::string &fsPath);

private:
    ImageManagerImpl() = default;
    ~ImageManagerImpl();

    struct Entry {
        ImageHandle                      resource;
        std::list<std::string>::iterator lru_pos;
    };

    ImageHandle acquire(const std::string &key, const std::function<ImageHandle()> &decoder);
    ImageHandle decodeAsset(const std::string &key);
    ImageHandle decodeFile(const std::string &key, const std::string &fsPath);
    ImageHandle decodeBytes(const std::string &key, const std::uint8_t *data, std::size_t size) const;

    void               touchLocked(Entry &entry);
    void               enforceLimitsLocked();
    void               workerLoop();
    static std::string normalizeKey(const std::string &key);

    mutable std::mutex                     cache_mutex_;
    std::list<std::string>                 lru_;
    std::unordered_map<std::string, Entry> cache_;
    std::size_t                            max_entries_ = 32;
    std::size_t                            max_bytes_   = 64ull * 1024 * 1024;
    std::size_t                            cache_bytes_ = 0;

    std::mutex                      queue_mutex_;
    std::condition_variable         queue_cv_;
    std::condition_variable         wait_cv_;
    std::deque<std::string>         queue_;
    std::unordered_set<std::string> queued_;
    bool                            worker_started_ = false;
    bool                            processing_     = false;
    bool                            stop_           = false;
    std::thread                     worker_;
};

} // namespace lvglpp::tools

#endif // LVGLPP_IMAGEMANAGERIMPL_H
