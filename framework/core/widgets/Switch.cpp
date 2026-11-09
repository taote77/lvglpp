
#include "Switch.h"
#include "config/color_sheet.h"

namespace lvglpp::widgets {

using namespace form::design;

Switch::Switch(lv_coord_t width, lv_coord_t height, Widget *parent) //
    : Widget(parent), width_(width), height_(height), check_state_(false)
{
    setSize(width, height);
    setRadius(18);
    round_item_ = std::make_unique<Widget>(this);
    round_item_->setSize(radius_ * 2, radius_ * 2);
    round_item_->setRadius(radius_);
    round_item_->setBgColor(CLR_PRIMARY_BRIGHT);
    round_item_->setEnabled(false);
    update();
    setFocusable(true);
}

void Switch::update()
{
    if (check_state_)
    {
        setBgColor(CLR_PRIMARY);
        round_item_->setAlignment(LV_ALIGN_RIGHT_MID, -2, 0);
    } else
    {
        setBgColor(CLR_PRIMARY_BLACK_HOVER);
        round_item_->setAlignment(LV_ALIGN_LEFT_MID, 2, 0);
    }
}

void Switch::setChecked(bool checked)
{
    check_state_ = checked;
    update();
}
} // namespace lvglpp::widgets
