
#ifndef LVGLPP_BASE64IMAGE_H
#define LVGLPP_BASE64IMAGE_H

#include "Widget.h"
#include <memory>
#include <string>

namespace lvglpp {

namespace widgets {

class Base64Image : public Widget
{
public:
    explicit Base64Image(std::string base64, int w, int h, Widget *parent);
    void updateImage(const std::string &data);

private:
    struct ImageState;

    std::shared_ptr<ImageState> image_state_;
    int                         w_;
    int                         h_;
    std::string                 base64Data_;
};

} // namespace widgets
} // namespace lvglpp

#endif // LVGLPP_BASE64IMAGE_H
