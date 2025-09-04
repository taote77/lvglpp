
#include "QrCode.h"
#include "color_sheet.h"

namespace lvglpp::widgets {

QrCode::QrCode(uint32_t length, lv_color_t back_color, lv_color_t front_color, BaseItem *parent) : BaseItem(parent), _length(length)
{
    this->setSize(length, length);

    _qr = lv_qrcode_create(getLvglItem());
    lv_qrcode_set_size(_qr, length);
    lv_qrcode_set_dark_color(_qr, front_color);
    lv_qrcode_set_light_color(_qr, back_color);

    const std::string data = "https://lvglpp.io";
    lv_qrcode_update(_qr, data.c_str(), data.length());
    lv_obj_center(_qr);

    lv_obj_set_style_border_color(_qr, back_color, 0);
    lv_obj_set_style_border_width(_qr, 5, 0);
}

QrCode::~QrCode()
{
    if (_qr != nullptr)
    {
        lv_obj_del(_qr);
        _qr = nullptr;
    }
}

void QrCode::setData(const std::string &data)
{
    lv_qrcode_update(_qr, data.c_str(), data.length());
}

void QrCode::setImgSrc(const std::string &src)
{
    if (!_img_icon)
    {
        _img_icon = std::make_shared<lvglpp::widgets::Image>("", this);
        _img_icon->setSize(_length / 4, _length / 4);
        _img_icon->setAligment(LV_ALIGN_CENTER, 0, 0);
    }
    _img_icon->setImageSrcPath(src);
}

} // namespace lvglpp::widgets
