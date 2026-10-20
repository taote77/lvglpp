#include "ImageManager.h"
#include "ImageManagerImpl.h"

namespace lvglpp::tools {

ImageManager::Handle ImageManager::load(const std::string &assetPath)
{
    return ImageManagerImpl::getInstance().load(assetPath);
}

ImageManager::Handle ImageManager::loadFile(const std::string &fsPath)
{
    return ImageManagerImpl::getInstance().loadFile(fsPath);
}

void ImageManager::prefetch(const std::string &assetPath)
{
    ImageManagerImpl::getInstance().prefetch(std::vector<std::string>{assetPath});
}

void ImageManager::prefetch(const std::vector<std::string> &assetPaths)
{
    ImageManagerImpl::getInstance().prefetch(assetPaths);
}

void ImageManager::waitForPrefetch()
{
    ImageManagerImpl::getInstance().waitForPrefetch();
}

void ImageManager::invalidate(const std::string &assetPath)
{
    ImageManagerImpl::getInstance().invalidate(ImageManagerImpl::assetKey(assetPath));
}

void ImageManager::invalidateFile(const std::string &fsPath)
{
    ImageManagerImpl::getInstance().invalidate(ImageManagerImpl::fileKey(fsPath));
}

void ImageManager::setCacheLimits(std::size_t maxEntries, std::size_t maxBytes)
{
    ImageManagerImpl::getInstance().setCacheLimits(maxEntries, maxBytes);
}

void ImageManager::clearCache()
{
    ImageManagerImpl::getInstance().clearCache();
}

std::size_t ImageManager::cacheSize()
{
    return ImageManagerImpl::getInstance().cacheSize();
}

std::size_t ImageManager::cacheBytes()
{
    return ImageManagerImpl::getInstance().cacheBytes();
}

bool ImageManager::isCached(const std::string &key)
{
    return ImageManagerImpl::getInstance().isCached(key);
}

void ImageManager::shutdown()
{
    ImageManagerImpl::getInstance().shutdown();
}

} // namespace lvglpp::tools
