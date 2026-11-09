#include "ToastView.h"

#include "config/color_sheet.h"
#include "core/base/LvObjFactory.h"

#include <algorithm>

namespace lvglpp::widgets {

namespace {
constexpr lv_coord_t kHeight       = 56;
constexpr lv_coord_t kIconSize     = 40;
constexpr lv_coord_t kPaddingHor   = 16;
constexpr lv_coord_t kPaddingVer   = 8;
constexpr lv_coord_t kIconGap      = 12;
constexpr lv_coord_t kTopOffset    = 70;
constexpr lv_coord_t kScreenMargin = 16;
constexpr lv_coord_t kMinTextWidth = 40;

const char *iconFor(ToastType type)
{
    switch (type)
    {
    case ToastType::Warning:
        return "image/40x40/icon_warn.png";
    case ToastType::Error:
        return "image/40x40/icon_error.png";
    case ToastType::Success:
        break;
    }
    return "image/40x40/icon_success.png";
}
} // namespace

ToastView::ToastView() : Widget(tools::LvObjFactory::createLvglDialog)
{
    lv_obj_clear_flag(lvObj(), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(lvObj(), LV_OBJ_FLAG_CLICKABLE);
    setRadius(8);
    setPaddingHor(kPaddingHor);
    setPaddingVer(kPaddingVer);
    setPaddingColumn(kIconGap);
    setFlexFlow(LV_FLEX_FLOW_ROW);
    setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    setAlignment(LV_ALIGN_TOP_MID, 0, kTopOffset);

    icon_ = std::make_unique<Image>("", this);
    icon_->setSize(kIconSize, kIconSize);

    desc_ = std::make_unique<Label>("", 0xFFFFFF, this, Label::FontSize24, Label::SansSC);
    applyTheme();
    setVisible(false);
    setOpacity(0.0);
}

void ToastView::setContent(ToastType type, const std::string &message)
{
    applyTheme();

    // Reset truncation state first: the previous message may have enabled it.
    desc_->setLongMode(0, Label::LongMode::None);
    desc_->setText(message);

    lv_display_t    *display      = lv_display_get_default();
    const lv_coord_t displayWidth = display ? lv_display_get_horizontal_resolution(display) : 0;
    const lv_coord_t chrome       = 2 * kPaddingHor + kIconSize + kIconGap;
    const lv_coord_t available    = displayWidth > 0 ? displayWidth - 2 * kScreenMargin - chrome : kMinTextWidth;
    const lv_coord_t maxTextWidth = std::max(kMinTextWidth, available);
    const lv_coord_t fullWidth    = desc_->getDisplayTextWidth();
    const lv_coord_t textWidth    = std::min(fullWidth, maxTextWidth);

    if (fullWidth > maxTextWidth)
    {
        desc_->setLongMode(maxTextWidth, Label::LongMode::MiddleEllipsis);
    }
    desc_->setWidth(textWidth);

    setSize(textWidth + chrome, kHeight);
    icon_->setImageSrcPath(iconFor(type));
}

void ToastView::applyTheme()
{
    // The view is a process-lifetime overlay, so it is not rebuilt by
    // UiConfig::requestRebuild(); refresh semantic colours on every show.
    setBgColor(form::design::CLR_PRIMARY_DARK_BG);
    if (desc_)
    {
        desc_->setTextColor(form::design::CLR_ON_SURFACE);
    }
}

} // namespace lvglpp::widgets
