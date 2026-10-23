#include "PushButton.h"
#include "config/color_sheet.h"
#include "core/style/Theme.h"
#include <memory>

namespace lvglpp::widgets {

using namespace form::design;

namespace {

core::ButtonStyle toThemeStyle(PushButton::ColorStyle type)
{
    switch (type)
    {
    case PushButton::Blue:
        return core::ButtonStyle::Blue;
    case PushButton::Dark:
        return core::ButtonStyle::Dark;
    case PushButton::Gray:
        return core::ButtonStyle::Gray;
    case PushButton::LightBlue:
        return core::ButtonStyle::LightBlue;
    case PushButton::BlackBroder:
        return core::ButtonStyle::BlackBorder;
    case PushButton::Red:
        return core::ButtonStyle::Red;
    case PushButton::DarkGray:
        return core::ButtonStyle::DarkGray;
    }
    return core::ButtonStyle::Blue;
}

} // namespace

PushButton::PushButton(int width, int height, PushButton::ColorStyle type, const std::string &strMsg, Widget *parent) : Widget(parent), type_(type)
{
    init(width, height, type);
    initLabel(strMsg, type);
    setColorType(type);
    setFocusable(true);
}

PushButton::PushButton(int width, int height, PushButton::ColorStyle type, const std::string &strMsg, const std::string &strImgPath, Widget *parent) :
    Widget(parent), type_(type)
{
    init(width, height, type);
    initImage(strImgPath);
    initLabel(strMsg, type);
    setColorType(type);
    setFocusable(true);
}

void PushButton::init(int width, int height, PushButton::ColorStyle type)
{
    setSize((lv_coord_t)width, (lv_coord_t)height);
    setRadius(8);
}

void PushButton::initLabel(const std::string &msg, PushButton::ColorStyle type)
{
    if (lvObj() == nullptr)
    {
        return;
    }
    label_.reset(new LvText(msg, CLR_PRIMARY_BLACK, this, LvText::FontSize20));
    label_->setAlignment(LV_ALIGN_CENTER, 0, 0);
}

void PushButton::setColorType(ColorStyle type)
{
    if (lvObj() == nullptr)
    {
        return;
    }

    type_                              = type;
    const core::ButtonPalette &palette = core::Theme::instance().buttonPalette(toThemeStyle(type));

    if (palette.transparent)
    {
        lv_obj_set_style_bg_opa(lvObj(), LV_OPA_TRANSP, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(lvObj(), 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(lvObj(), lv_color_hex(palette.border_color), LV_STATE_DEFAULT);
    } else
    {
        lv_obj_set_style_bg_color(lvObj(), lv_color_hex(palette.normal_bg), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(lvObj(), lv_color_hex(palette.pressed_bg), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(lvObj(), lv_color_hex(palette.disabled_bg), LV_STATE_DISABLED);
    }

    if (label_ != nullptr)
    {
        label_->setTextClr(palette.label_color);
    }
}

void PushButton::initImage(const std::string &url)
{
    if (lvObj() == nullptr)
    {
        return;
    }
    img_obj_ = std::make_unique<widgets::Image>(url, this);
    img_obj_->setAlignment(LV_ALIGN_CENTER, 0, 0);
}

void PushButton::setImagePos(int x, int y)
{
    if (img_obj_ == nullptr)
    {
        return;
    }
    img_obj_->setPos(x, y);
}

void PushButton::setTextPos(int x, int y)
{
    if (label_ == nullptr)
    {
        return;
    }
    label_->setPos((lv_coord_t)x, (lv_coord_t)y);
}

void PushButton::setImagePos(lv_align_t align, int x, int y)
{
    if (img_obj_ == nullptr)
    {
        return;
    }
    img_obj_->setAlignment(align, (lv_coord_t)x, (lv_coord_t)y);
}

void PushButton::setTextPos(lv_align_t align, int x, int y)
{
    if (label_ == nullptr)
    {
        return;
    }
    label_->setAlignment(align, (lv_coord_t)x, (lv_coord_t)y);
}

void PushButton::setTextAlignment(lv_text_align_t align_type)
{
    if (label_ != nullptr)
    {
        label_->setTextAlign(align_type);
    }
}

void PushButton::setTextMsg(const std::string &str)
{
    if (label_ == nullptr)
    {
        return;
    }
    label_->setText(str);
}

void PushButton::setEnabled(bool enable)
{
    Widget::setEnabled(enable);
    if (label_ != nullptr)
    {
        const core::ButtonPalette &palette = core::Theme::instance().buttonPalette(toThemeStyle(type_));
        label_->setTextClr(enable ? palette.label_color : palette.label_disabled);
    }
}

void PushButton::changeImageAsset(const std::string &assetPath)
{
    if (img_obj_ != nullptr)
    {
        img_obj_->setAsset(assetPath);
    }
}

void PushButton::setOnClickedListener(const std::function<void()> &l)
{
    onButtonClicked_cb_ = l;
    Widget::setOnClickedListener(std::bind(&PushButton::handleButtonClicked, this));
}

void PushButton::handleButtonClicked()
{
    if (repeat_ms_ > 0)
    {
        auto now_point = std::chrono::system_clock::now();
        auto count     = std::chrono::duration<double, std::milli>(now_point - prev_time_point_).count();
        if (std::fabs(count) <= repeat_ms_)
        {
            return;
        }

        prev_time_point_ = now_point;
    }
    if (onButtonClicked_cb_ != nullptr)
    {
        onButtonClicked_cb_();
    }
}
} // namespace lvglpp::widgets
