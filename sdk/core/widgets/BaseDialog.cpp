#include "BaseDialog.h"
#include "config/color_sheet.h"
#include "core/base/LvObjFactory.h"
#include "core/sys/InputManager.h"
#include "core/sys/UiConfig.h"
#include <unistd.h>

namespace lvglpp::widgets {
Widget *BaseDialog::getRoot() const
{
    return background_item_ptr_.get();
}

BaseDialog::BaseDialog(bool is_sys_dialog) //
    : background_item_ptr_(new Widget(is_sys_dialog ? tools::LvObjFactory::createLvglSysDialog : tools::LvObjFactory::createLvglDialog))
{
    background_item_ptr_->setOpacity(1);
    background_item_ptr_->setSize(sys::UiConfig::instance().width(), sys::UiConfig::instance().height());
    background_item_ptr_->setRadius(0);
    background_item_ptr_->setVisible(false);

#if LVGLPP_DYNAMIC_UI
    // A configuration change invalidates absolute layouts. Close transient
    // dialogs; the owner can reopen them with the new configuration.
    config_connection_ = core::ScopedConnection(core::Object::connect(
        &sys::UiConfig::instance(), "configurationChanged", [this]() { close(); }));
#endif
}

BaseDialog::~BaseDialog()
{
    close();
}

void BaseDialog::open()
{
    if (!background_item_ptr_ || background_item_ptr_->isVisible())
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
    return background_item_ptr_->isVisible();
}
} // namespace lvglpp::widgets
