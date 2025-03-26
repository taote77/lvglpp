
#ifndef LV_BASE64_IMAGE_H
#define LV_BASE64_IMAGE_H

#include "BaseItem.h"
#include <string>

namespace lvglpp {
namespace widgets {
class Base64Image : public BaseItem
{
public:
    explicit Base64Image(std::string base64, int w, int h, BaseItem *parent);
    ~Base64Image() override;
    void updateImage(const std::string &data);

private:
    lv_image_dsc_t img_struct_{};
    int w_;
    int h_;
    std::string base64Data_;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_BASE64_IMAGE_H
