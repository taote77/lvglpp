#include "QrCode.h"
#include "config/color_sheet.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

QrCode::QrCode(uint32_t length, lv_color_t back_color, lv_color_t front_color, Widget *parent) : Widget(tools::LvObjFactory::createLvglQrCode, parent), _length(length)
{
    setSize(length, length);

    lv_obj_t *qr = lvObj();
    if (qr == nullptr)
    {
        return;
    }

    lv_qrcode_set_size(qr, length);
    lv_qrcode_set_dark_color(qr, front_color);
    lv_qrcode_set_light_color(qr, back_color);

    const std::string data = "https://lvglpp.io";
    lv_qrcode_update(qr, data.c_str(), static_cast<uint32_t>(data.length()));
    lv_obj_center(qr);

    lv_obj_set_style_border_color(qr, back_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
}

void QrCode::setData(const std::string &data)
{
    if (lvObj() != nullptr)
    {
        lv_qrcode_update(lvObj(), data.c_str(), static_cast<uint32_t>(data.length()));
    }
}

void QrCode::setImgSrc(const std::string &assetPath)
{
    if (_img_icon == nullptr)
    {
        _img_icon = std::make_unique<lvglpp::widgets::Image>("", this);
        _img_icon->setSize(_length / 4, _length / 4);
        _img_icon->setAlignment(LV_ALIGN_CENTER, 0, 0);
    }
    _img_icon->setAsset(assetPath);
}

void QrCode::setImgFile(const std::string &fsPath)
{
    if (_img_icon == nullptr)
    {
        _img_icon = std::make_unique<lvglpp::widgets::Image>("", this);
        _img_icon->setSize(_length / 4, _length / 4);
        _img_icon->setAlignment(LV_ALIGN_CENTER, 0, 0);
    }
    _img_icon->setFile(fsPath);
}

} // namespace lvglpp::widgets
