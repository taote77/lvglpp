#ifndef LVGLPP_BASE64IMAGE_H
#define LVGLPP_BASE64IMAGE_H

#include "Widget.h"
#include <memory>
#include <string>

namespace lvglpp::widgets {

/// Image widget whose source is a base64-encoded bitmap. The decoded image is
/// scaled to fit width x height (preserving aspect ratio) and centered.
class Base64Image : public Widget
{
public:
    Base64Image(std::string base64, int width, int height, Widget *parent);

    /// Replace the image data. Invalid data leaves the current image intact.
    void setBase64(const std::string &data);

    [[deprecated("use setBase64()")]]
    void updateImage(const std::string &data)
    {
        setBase64(data);
    }

private:
    struct ImageState;

    void applyDecoded(const unsigned char *rgba, int sourceWidth, int sourceHeight);

    std::shared_ptr<ImageState> image_state_;
    int                         width_;
    int                         height_;
    std::string                 base64_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_BASE64IMAGE_H
