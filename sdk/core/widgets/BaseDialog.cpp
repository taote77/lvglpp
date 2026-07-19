#include "core/kernel/LvObjFactory.h"
#include "BaseDialog.h"
#include "config/color_sheet.h"
#include <unistd.h>

namespace lvglpp::widgets {
BaseItem *BaseDialog::getRoot() const
{
    return background_item_ptr_.get();
}

BaseDialog::BaseDialog(bool is_sys_dialog) //
    : background_item_ptr_(new BaseItem(is_sys_dialog ? tools::LvObjFactory::createLvglSysDialog : tools::LvObjFactory::createLvglDialog))
{
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
}

bool BaseDialog::isOpen()
{
    return background_item_ptr_->getVisible();
}
} // namespace lvglpp::widgets
