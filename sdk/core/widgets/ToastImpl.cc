#include "ToastImpl.h"
#include "color_sheet.h"

namespace lvglpp::widgets {
ToastImpl::ToastImpl() : BaseItem(BaseItem::Dialog)
{
    lv_obj_clear_flag(getLvglItem(), LV_OBJ_FLAG_SCROLLABLE);
    setBgColor(0x2C3138);
    setRadius(8);
    setAligment(LV_ALIGN_TOP_MID, 0, 70);
    icon_ = std::make_shared<widgets::Image>("", this);
    icon_->setAligment(LV_ALIGN_LEFT_MID, 8, 0);
    desc_ = std::make_shared<LvText>("", 0xFFFFFF, this, LvText::FontSize24, LvText::SansSC);
    desc_->setAligment(LV_ALIGN_LEFT_MID, 64, 0);
}

void ToastImpl::setMessage(const std::string &str_msg, ToastImpl::IconType type)
{
    // setSize(str_msg.length()*12+80,56);
    desc_->setText(str_msg);
    setSize(desc_->getDisplayTextWidth() + 72, 56);
    if (type == ToastImpl::Succeed)
    {
        icon_->setImageSrcPath(":/image/40x40/icon_success.png");
    } else if (type == ToastImpl::Warn)
    {
        icon_->setImageSrcPath(":/image/40x40/icon_warn.png");
    } else
    {
        icon_->setImageSrcPath(":/image/40x40/icon_error.png");
    }
}
} // namespace lvglpp::widgets