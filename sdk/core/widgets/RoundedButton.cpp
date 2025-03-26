
#include <boost/algorithm/string/predicate.hpp>
#include "RoundedButton.h"
#include "GlobalVar.h"
#include "core/tools/Utils.h"
#include "core/tools/LvglUtils.h"
#include "core/tools/ImageManager.h"

namespace lvglpp {
namespace widgets {

RoundedButton::RoundedButton(int width, int height, RoundedButton::ColorStyle type,
                             const std::string &strMsg, BaseItem *parent)
    : BaseItem(parent), type_(type)
{
    init(width, height, type);
    if (boost::starts_with(strMsg, ":/")) {
        // 如果是图片资源
        initImage(strMsg);
    } else {
        initLabel(strMsg, type);
    }
    setColorType(type);
}

RoundedButton::RoundedButton(int width, int height, RoundedButton::ColorStyle type,
                             const std::string &strMsg, const std::string &strImgPath,
                             BaseItem *parent)
    : BaseItem(parent), type_(type)
{
    init(width, height, type);
    initImage(strImgPath);
    initLabel(strMsg, type);
    setColorType(type);
}

void RoundedButton::init(int width, int height, RoundedButton::ColorStyle type)
{
    setSize((lv_coord_t)width, (lv_coord_t)height);
    setRadius(8);
}

void RoundedButton::initLabel(const std::string &msg, RoundedButton::ColorStyle type)
{
    if (getLvglItem() == nullptr) {
        return;
    }
    label_.reset(new LvText(msg, CLR_PRIMARY_BLACK, this, LvText::FontSize24));
    label_->setAligment(LV_ALIGN_CENTER, 0, 0);
}

void RoundedButton::setColorType(ColorStyle type)
{
    if (getLvglItem() == nullptr) {
        return;
    }
    if (type == RoundedButton::Gray) {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xD8DCE2), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK_HOVER),
                                  LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xD8DCE2), LV_STATE_DISABLED);
        if (label_ != nullptr)
            label_->setTextClr(CLR_PRIMARY_BLACK);
    } else if (type == RoundedButton::Dark) {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_DARK_BG),
                                  LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK_HOVER),
                                  LV_STATE_DISABLED);

        if (label_ != nullptr)
            label_->setTextClr(CLR_PRIMARY_BRIGHT);
    } else if (type == RoundedButton::LightBlue) {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xF7F9FF), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xEAEFFF), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0xF7F9FF), LV_STATE_DISABLED);

        if (label_ != nullptr)
            label_->setTextClr(CLR_PRIMARY_BLUE);
    } else if (type == RoundedButton::BlackBroder) {
        lv_obj_set_style_bg_opa(getLvglItem(), LV_OPA_TRANSP, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(getLvglItem(), 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_DIS_BG),
                                      LV_STATE_DEFAULT);

        if (label_ != nullptr)
            label_->setTextClr(CLR_PRIMARY_BLACK);
    } else if (type == RoundedButton::Red) {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_WARNING_DARK), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(0x661616), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLACK_HOVER),
                                  LV_STATE_DISABLED);

        if (label_ != nullptr)
            label_->setTextClr(CLR_PRIMARY_BRIGHT);
    } else {
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLUE), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_DARK_BLUE),
                                  LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(getLvglItem(), lv_color_hex(CLR_PRIMARY_BLUE_DIS_TEXT),
                                  LV_STATE_DISABLED);

        if (label_ != nullptr)
            label_->setTextClr(CLR_PRIMARY_BRIGHT);
    }
}

void RoundedButton::initImage(const std::string &url)
{
    if (getLvglItem() == nullptr) {
        return;
    }
    img_obj_.reset(new widgets::Image(url, this));
    img_obj_->setAligment(LV_ALIGN_CENTER, 0, 0);
}

void RoundedButton::setImagePos(int x, int y)
{
    if (img_obj_ == nullptr) {
        return;
    }
    img_obj_->setPos(x, y);
}

void RoundedButton::setTextPos(int x, int y)
{
    if (label_ == nullptr) {
        return;
    }
    label_->setPos((lv_coord_t)x, (lv_coord_t)y);
}

void RoundedButton::setImagePos(lv_align_t align, int x, int y)
{
    if (img_obj_ == nullptr) {
        return;
    }
    img_obj_->setAligment(align, (lv_coord_t)x, (lv_coord_t)y);
}

void RoundedButton::setTextPos(lv_align_t align, int x, int y)
{
    if (label_ == nullptr) {
        return;
    }
    label_->setAligment(align, (lv_coord_t)x, (lv_coord_t)y);
}

void RoundedButton::setTextAlignment(lv_text_align_t align_type)
{
    lv_style_set_text_align(label_->getTextStyle(), align_type);
}

void RoundedButton::setTextMsg(const std::string &str)
{
    if (label_ == nullptr) {
        return;
    }
    label_->setText(str);
}

void RoundedButton::setEnable(bool enable)
{
    BaseItem::setEnable(enable);
    if (type_ == RoundedButton::Gray) {
        if (label_ != nullptr) {
            label_->setTextClr(enable ? CLR_PRIMARY_BLACK : CLR_PRIMARY_DIS_BG);
        }
    }
}

void RoundedButton::changeImageUrl(const std::string &url)
{
    if (img_obj_ != nullptr) {
        img_obj_->setImageSrcPath(url);
    }
}

void RoundedButton::setOnClickedListener(const std::function<void()> &l)
{
    onButtonClicked_cb_ = l;
    BaseItem::setOnClickedListener(std::bind(&RoundedButton::handleButtonClicked, this));
}

void RoundedButton::handleButtonClicked()
{
    if (repeat_ms_ > 0) {
        auto now_point = std::chrono::system_clock::now();
        auto count =
                std::chrono::duration<double, std::milli>(now_point - prev_time_point_).count();
        if (std::fabs(count) <= repeat_ms_) {
            return;
        }

        prev_time_point_ = now_point;
    }
    if (onButtonClicked_cb_ != nullptr) {
        onButtonClicked_cb_();
    }
}
} // namespace widgets
} // namespace lvglpp
