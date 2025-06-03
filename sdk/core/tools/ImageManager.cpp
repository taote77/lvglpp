
#include "ImageManager.h"
#include "ImageManagerImpl.h"
namespace lvglpp {
namespace tools {
lv_img_dsc_t *ImageManager::getImageDscByUrl(const std::string &url)
{
    return ImageManagerImpl::getInstance()->getImageDscByUrl(url);
}

} // namespace tools
} // namespace lvglpp