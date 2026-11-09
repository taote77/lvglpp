#ifndef LVGLPP_QRCODE_H
#define LVGLPP_QRCODE_H

#include "Widget.h"
#include "Image.h"
#include <memory>

namespace lvglpp::widgets {

class QrCode : public Widget
{
public:
    explicit QrCode(uint32_t length, lv_color_t back_color, lv_color_t front_color, Widget *parent = nullptr);

    void setData(const std::string &data);

    /// Set the center icon from an application asset path.
    void setImgSrc(const std::string &assetPath);

    /// Set the center icon from a real filesystem path.
    void setImgFile(const std::string &fsPath);

private:
    std::unique_ptr<widgets::Image> _img_icon;
    uint32_t                        _length = 0;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_QRCODE_H
