#include "ImageManagerImpl.h"

#include "AssetManager.h"
#include "core/log/log.h"
#include "core/tools/stb/StbImageLock.h"
#include "core/tools/stb/stb_image.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <utility>

namespace lvglpp::tools {

namespace {

constexpr std::uint64_t kMaxDimension    = 16384;
constexpr std::uint64_t kMaxDecodedBytes = 256ull * 1024 * 1024;

} // namespace

ImageManagerImpl &ImageManagerImpl::getInstance()
{
    static ImageManagerImpl instance;
    return instance;
}

ImageManagerImpl::~ImageManagerImpl()
{
    shutdown();
}

std::string ImageManagerImpl::normalizeKey(const std::string &key)
{
    if (key.rfind("file:", 0) == 0)
    {
        return key;
    }

    std::string normalized = key;
    if (normalized.size() >= 2 && normalized[0] == 'A' && normalized[1] == ':')
    {
        normalized.erase(0, 2);
    }
    while (!normalized.empty() && normalized.front() == '/')
    {
        normalized.erase(normalized.begin());
    }
    return normalized;
}

std::string ImageManagerImpl::assetKey(const std::string &assetPath)
{
    return normalizeKey(assetPath);
}

std::string ImageManagerImpl::fileKey(const std::string &fsPath)
{
    std::error_code ec;
    const auto      absolute = std::filesystem::absolute(fsPath, ec);
    return "file:" + (ec ? fsPath : absolute.lexically_normal().string());
}

ImageHandle ImageManagerImpl::load(const std::string &assetPath)
{
    const std::string key = assetKey(assetPath);
    return acquire(key, [this, key]() { return decodeAsset(key); });
}

ImageHandle ImageManagerImpl::loadFile(const std::string &fsPath)
{
    const std::string key = fileKey(fsPath);
    return acquire(key, [this, key, fsPath]() { return decodeFile(key, fsPath); });
}

ImageHandle ImageManagerImpl::acquire(const std::string &key, const std::function<ImageHandle()> &decoder)
{
    {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        const auto                  cached = cache_.find(key);
        if (cached != cache_.end())
        {
            touchLocked(cached->second);
            return cached->second.resource;
        }
    }

    ImageHandle resource = decoder();
    if (!resource)
    {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(cache_mutex_);
    const auto                  cached = cache_.find(key);
    if (cached != cache_.end())
    {
        // Another thread decoded the same image while we were working.
        touchLocked(cached->second);
        return cached->second.resource;
    }

    lru_.push_front(key);
    cache_.emplace(key, Entry{resource, lru_.begin()});
    cache_bytes_ += resource->byteSize();
    enforceLimitsLocked();
    return resource;
}

ImageHandle ImageManagerImpl::decodeAsset(const std::string &key)
{
    AssetHandle asset = AssetManager::getInstance().open(key);
    if (!asset)
    {
        return nullptr;
    }
    return decodeBytes(key, asset->data(), asset->size());
}

ImageHandle ImageManagerImpl::decodeFile(const std::string &key, const std::string &fsPath)
{
    std::ifstream input(fsPath, std::ios::binary | std::ios::ate);
    if (!input)
    {
        LOG_ERROR() << "cannot open image file: " << fsPath;
        return nullptr;
    }

    const std::streamoff length = input.tellg();
    if (length <= 0)
    {
        LOG_ERROR() << "cannot read image file: " << fsPath;
        return nullptr;
    }
    input.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(length));
    if (!input.read(reinterpret_cast<char *>(bytes.data()), length))
    {
        LOG_ERROR() << "cannot read image file: " << fsPath;
        return nullptr;
    }
    return decodeBytes(key, bytes.data(), bytes.size());
}

ImageHandle ImageManagerImpl::decodeBytes(const std::string &key, const std::uint8_t *data, std::size_t size) const
{
    if (data == nullptr || size == 0)
    {
        return nullptr;
    }
    if (size > static_cast<std::size_t>(INT_MAX))
    {
        LOG_ERROR() << "image is too large to decode: " << key;
        return nullptr;
    }

    std::lock_guard<std::mutex> stbLock(stbImageMutex());

    int            width    = 0;
    int            height   = 0;
    int            channels = 0;
    unsigned char *rgba     = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &channels, 4);
    if (rgba == nullptr)
    {
        const char *reason = stbi_failure_reason();
        LOG_ERROR() << "image decode failed: " << key << ": " << (reason != nullptr ? reason : "unknown");
        return nullptr;
    }
    std::unique_ptr<unsigned char, void (*)(void *)> guard(rgba, stbi_image_free);

    if (width <= 0 || height <= 0 || static_cast<std::uint64_t>(width) > kMaxDimension || static_cast<std::uint64_t>(height) > kMaxDimension)
    {
        LOG_ERROR() << "invalid image dimensions for " << key << ": " << width << "x" << height;
        return nullptr;
    }

    const std::uint64_t byteCount = static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height) * 4ull;
    if (byteCount > kMaxDecodedBytes)
    {
        LOG_ERROR() << "decoded image is too large: " << key << " (" << byteCount << " bytes)";
        return nullptr;
    }

    auto pixels = std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(byteCount));
    std::memcpy(pixels.get(), rgba, static_cast<std::size_t>(byteCount));

    // stb returns RGBA; LVGL ARGB8888 stores BGRA in memory.
    for (std::size_t i = 0; i + 3 < static_cast<std::size_t>(byteCount); i += 4)
    {
        std::swap(pixels[i], pixels[i + 2]);
    }

    return std::make_shared<ImageResource>(key, width, height, std::move(pixels));
}

void ImageManagerImpl::touchLocked(Entry &entry)
{
    lru_.splice(lru_.begin(), lru_, entry.lru_pos);
    entry.lru_pos = lru_.begin();
}

void ImageManagerImpl::enforceLimitsLocked()
{
    while (!lru_.empty() && (cache_.size() > max_entries_ || cache_bytes_ > max_bytes_))
    {
        const std::string victim = lru_.back();
        lru_.pop_back();

        const auto cached = cache_.find(victim);
        if (cached != cache_.end())
        {
            const std::size_t bytes = cached->second.resource->byteSize();
            cache_bytes_            = (cache_bytes_ > bytes) ? (cache_bytes_ - bytes) : 0;
            cache_.erase(cached);
        }
    }
}

void ImageManagerImpl::prefetch(const std::vector<std::string> &assetPaths)
{
    if (assetPaths.empty())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (!worker_started_)
    {
        stop_           = false;
        worker_         = std::thread(&ImageManagerImpl::workerLoop, this);
        worker_started_ = true;
    }

    for (const auto &assetPath : assetPaths)
    {
        const std::string key = assetKey(assetPath);
        if (isCached(key))
        {
            continue;
        }
        if (queued_.insert(key).second)
        {
            queue_.push_back(key);
        }
    }
    queue_cv_.notify_all();
}

void ImageManagerImpl::waitForPrefetch()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    wait_cv_.wait(lock, [this]() { return queue_.empty() && !processing_; });
}

void ImageManagerImpl::workerLoop()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_cv_.wait(lock, [this]() { return stop_ || !queue_.empty(); });

        if (queue_.empty())
        {
            processing_ = false;
            wait_cv_.notify_all();
            break;
        }

        std::string path = std::move(queue_.front());
        queue_.pop_front();
        queued_.erase(path);
        processing_ = true;
        lock.unlock();

        load(path);

        lock.lock();
        processing_ = false;
        if (queue_.empty())
        {
            wait_cv_.notify_all();
        }
        if (stop_ && queue_.empty())
        {
            break;
        }
    }
}

void ImageManagerImpl::invalidate(const std::string &key)
{
    const std::string normalized = normalizeKey(key);

    std::lock_guard<std::mutex> lock(cache_mutex_);
    const auto                  cached = cache_.find(normalized);
    if (cached == cache_.end())
    {
        return;
    }

    const std::size_t bytes = cached->second.resource->byteSize();
    cache_bytes_            = (cache_bytes_ > bytes) ? (cache_bytes_ - bytes) : 0;
    lru_.erase(cached->second.lru_pos);
    cache_.erase(cached);
}

void ImageManagerImpl::setCacheLimits(std::size_t maxEntries, std::size_t maxBytes)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    max_entries_ = maxEntries;
    max_bytes_   = maxBytes;
    enforceLimitsLocked();
}

void ImageManagerImpl::clearCache()
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    cache_.clear();
    lru_.clear();
    cache_bytes_ = 0;
}

std::size_t ImageManagerImpl::cacheSize() const
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    return cache_.size();
}

std::size_t ImageManagerImpl::cacheBytes() const
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    return cache_bytes_;
}

bool ImageManagerImpl::isCached(const std::string &key) const
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    return cache_.find(normalizeKey(key)) != cache_.end();
}

void ImageManagerImpl::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
        queue_.clear();
        queued_.clear();
        wait_cv_.notify_all();
    }
    queue_cv_.notify_all();

    if (worker_.joinable())
    {
        worker_.join();
    }

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        worker_started_ = false;
        processing_     = false;
        stop_           = false;
    }

    clearCache();
}

} // namespace lvglpp::tools
