
#include "ImageManager.h"
#include "ImageManagerImpl.h"
namespace heygears
{
    namespace tools
    {
        lv_img_dsc_t *ImageManager::getImageDscByUrl(const std::string& url)
        {
            return ImageManagerImpl::getInstance()->getImageDscByUrl(url);
        }

        std::string ImageManager::getImageFileDataByUrl(const std::string& url)
        {
            return ImageManagerImpl::getInstance()->getImageFileDataByUrl(url);
        }
    } // heygears
} // tools