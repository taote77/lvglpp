
#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H
#include "lvgl.h"
#include <string>
namespace lvglpp {
namespace tools {

class ImageManager
{
public:
    static lv_img_dsc_t *getImageDscByUrl(const std::string &url);

private:
};

} // namespace tools
} // namespace lvglpp

#endif // IMAGEMANAGER_H
