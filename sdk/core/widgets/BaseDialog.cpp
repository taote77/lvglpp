#include "BaseDialog.h"
#include "config/color_sheet.h"
#include "core/base/LvObjFactory.h"
#include "core/sys/InputManager.h"
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

BaseDialog::~BaseDialog()
{
    close();
}

void BaseDialog::open()
{
    if (!background_item_ptr_ || background_item_ptr_->getVisible())
    {
        return;
    }

    background_item_ptr_->setVisible(true);

    if (focus_scope_ == 0)
    {
        focus_scope_ = sys::InputManager::getInstance().pushModalScope(background_item_ptr_.get(), [this]() { close(); });
    }
}

void BaseDialog::close()
{
    if (focus_scope_ != 0)
    {
        sys::InputManager::getInstance().popScope(focus_scope_);
        focus_scope_ = 0;
    }

    if (background_item_ptr_)
    {
        background_item_ptr_->setVisible(false);
    }
}

bool BaseDialog::isOpen()
{
    return background_item_ptr_->getVisible();
}
} // namespace lvglpp::widgets
