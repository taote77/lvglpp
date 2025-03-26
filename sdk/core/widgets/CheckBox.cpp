
#include "CheckBox.h"
#include "GlobalVar.h"

namespace heygears {
namespace widgets {

CheckBox::CheckBox(int width, int height, uint32_t bg_clr, uint32_t check_clr, BaseItem *parent)
    : BaseItem(parent)
{
    check_item_ = std::make_shared<BaseItem>(this);
    check_item_->setSize(width - 8, height - 8);
    check_item_->setBgColor(check_clr);
    check_item_->setPos(2, 2);
    check_item_->setRadius(2);
    check_item_->setVisible(check_state_);

    setSize(width, height);
    setBgColor(bg_clr);
    setRadius(4);
    setBorder(2, CLR_PRIMARY_DIS_BG);

    init();
}

bool CheckBox::getCheckState()
{
    return check_state_;
}

void CheckBox::init()
{
    check_item_->setOnClickedListener([this]() -> void {
        check_state_ = !check_state_;
        check_item_->setVisible(check_state_);
    });

    setOnClickedListener([this]() -> void {
        check_state_ = !check_state_;
        check_item_->setVisible(check_state_);
    });
}
} // namespace widgets
} // namespace heygears