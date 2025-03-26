#include <unistd.h>
#include "BaseDialog.h"
#include "GlobalVar.h"

namespace lvglpp {
namespace widgets {
BaseItem *BaseDialog::getRootItem() const
{
    return background_item_ptr_.get();
}

BaseDialog::BaseDialog(bool is_sys_dialog)
    : background_item_ptr_(new BaseItem(is_sys_dialog ? BaseItem::SysDialog : BaseItem::Dialog))
{
    background_item_ptr_->setBgColor(CLR_PRIMARY_BLACK);
    background_item_ptr_->setOpacity(1);
    background_item_ptr_->setSize(800, 480);
    background_item_ptr_->setRadius(0);
    background_item_ptr_->setVisible(false);
}

void BaseDialog::open()
{
    background_item_ptr_->setVisible(true);
}

void BaseDialog::close()
{
    background_item_ptr_->setVisible(false);
    if (auto_del_) {
        delete this;
    }
}

bool BaseDialog::isOpen()
{
    return background_item_ptr_->getVisible();
}
} // namespace widgets
} // namespace lvglpp
