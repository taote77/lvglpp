#include "PushButton.h"
#include "color_sheet.h"
#include <memory>

namespace lvglpp::widgets {

using namespace form::design;

PushButton::PushButton(int width, int height, PushButton::ColorStyle type, const std::string &strMsg, BaseItem *parent) : BaseItem(parent), type_(type)
{
    init(width, height, type);
    if (strMsg.size() >= 2 && strMsg[0] == ':' && strMsg[1] == '/')
    {
        // 如果是图片资源
        initImage(strMsg);
    } else
    {
        initLabel(strMsg, type);
    }
    setColorType(type);
}

PushButton::PushButton(int width, int height, PushButton::ColorStyle type, const std::string &strMsg, const std::string &strImgPath, BaseItem *parent) :
    BaseItem(parent), type_(type)
{
    init(width, height, type);
    initImage(strImgPath);
    initLabel(strMsg, type);
    setColorType(type);
}

void PushButton::init(int width, int height, PushButton::ColorStyle type)
{
    setSize((lv_coord_t)width, (lv_coord_t)height);
    setRadius(8);
}

void PushButton::initLabel(const std::string &msg, PushButton::ColorStyle type)
{
    if (getLvglItem() == nullptr)
    {
        return;
    }
    label_.reset(new LvText(msg, CLR_PRIMARY_BLACK, this, LvText::FontSize20));
    label_->setAligment(LV_ALIGN_CENTER, 0, 0);
}

void PushButton::setColorType(ColorStyle type)
{
    if (getLvglItem() == nullptr)
    {
        return;
    }
    if (type == PushButton::Blue)
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_WIDGET_BUTTON_BLUE), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_WIDGET_BUTTON_BLUE_DIS), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_SURFACE_DIS_CONTAINER_1), LV_STATE_DISABLED);
        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_ON_SURFACE);
        }
    } else if (type == PushButton::Gray)
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_SECONDARY), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_WIDGET_BUTTON_GRAY_DIS), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_SURFACE_DIS_CONTAINER_1), LV_STATE_DISABLED);
        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_ON_SURFACE);
        }
    } else if (type == PushButton::DarkGray)
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_SECONDARY_CONTAINER), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK_HOVER), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_SURFACE_DIS_CONTAINER_1), LV_STATE_DISABLED);
        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_ON_SURFACE);
        }
    } else if (type == PushButton::Dark)
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_DARK_BG), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK_HOVER), LV_STATE_DISABLED);

        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_ON_SURFACE);
        }
    } else if (type == PushButton::LightBlue)
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xF7F9FF), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xEAEFFF), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xF7F9FF), LV_STATE_DISABLED);

        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_PRIMARY_BLUE);
        }
    } else if (type == PushButton::BlackBroder)
    {
        lv_obj_set_style_bg_opa(getLvglItem(), LV_OPA_TRANSP, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(getLvglItem(), 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_DIS_BG), LV_STATE_DEFAULT);

        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_PRIMARY_BLACK);
        }
    } else if (type == PushButton::Red)
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_WARNING_DARK), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0x661616), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK_HOVER), LV_STATE_DISABLED);

        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_ON_SURFACE);
        }
    } else
    {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_WIDGET_BUTTON_BLUE), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_DARK_BLUE), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLUE_DIS_TEXT), LV_STATE_DISABLED);

        if (label_ != nullptr)
        {
            label_->setTextClr(CLR_ON_SURFACE);
        }
    }
}

void PushButton::initImage(const std::string &url)
{
    if (getLvglItem() == nullptr)
    {
        return;
    }
    img_obj_ = std::make_unique<widgets::Image>(url, this);
    img_obj_->setAligment(LV_ALIGN_CENTER, 0, 0);
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
    img_obj_->setAligment(align, (lv_coord_t)x, (lv_coord_t)y);
}

void PushButton::setTextPos(lv_align_t align, int x, int y)
{
    if (label_ == nullptr)
    {
        return;
    }
    label_->setAligment(align, (lv_coord_t)x, (lv_coord_t)y);
}

void PushButton::setTextAlignment(lv_text_align_t align_type)
{
    lv_style_set_text_align(label_->getTextStyle(), align_type);
}

void PushButton::setTextMsg(const std::string &str)
{
    if (label_ == nullptr)
    {
        return;
    }
    label_->setText(str);
}

void PushButton::setEnable(bool enable)
{
    BaseItem::setEnable(enable);
    if (type_ == PushButton::Gray)
    {
        if (label_ != nullptr)
        {
            label_->setTextClr(enable ? CLR_PRIMARY_BLACK : CLR_PRIMARY_DIS_BG);
        }
    }
}

void PushButton::changeImageUrl(const std::string &url)
{
    if (img_obj_ != nullptr)
    {
        img_obj_->setImageSrcPath(url);
    }
}

void PushButton::setOnClickedListener(const std::function<void()> &l)
{
    onButtonClicked_cb_ = l;
    BaseItem::setOnClickedListener(std::bind(&PushButton::handleButtonClicked, this));
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
