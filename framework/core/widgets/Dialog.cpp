#include "Dialog.h"
#include "config/color_sheet.h"
#include "core/base/LvObjFactory.h"
#include "core/sys/UiConfig.h"

namespace lvglpp::widgets {

Dialog::Dialog(Layer layer) :
    background_(std::make_unique<Widget>(layer == Layer::System ? tools::LvObjFactory::createLvglSysDialog : tools::LvObjFactory::createLvglDialog)),
    priority_(layer == Layer::System ? OverlayManager::Priority::SystemDialog : OverlayManager::Priority::Dialog)
{
    background_->setOpacity(1);
    background_->setSize(sys::UiConfig::instance().width(), sys::UiConfig::instance().height());
    background_->setRadius(0);
    background_->setVisible(false);

#if LVGLPP_DYNAMIC_UI
    // A configuration change invalidates absolute layouts. Close transient
    // dialogs; the owner can reopen them with the new configuration.
    config_connection_ = core::ScopedConnection(core::Object::connect(&sys::UiConfig::instance(), "configurationChanged", [this]() { close(); }));
#endif
}

Dialog::~Dialog()
{
    close();
}

Widget *Dialog::getRoot() const
{
    return background_.get();
}

void Dialog::open()
{
    if (!background_ || background_->isVisible())
    {
        return;
    }

    background_->setVisible(true);

    if (!overlay_)
    {
        overlay_ = OverlayManager::getInstance().attach(background_.get(), priority_, [this]() {
            close();
            return true;
        });
    }
}

void Dialog::close()
{
    if (overlay_)
    {
        OverlayManager::getInstance().detach(overlay_);
        overlay_ = {};
    }

    if (background_)
    {
        background_->setVisible(false);
    }
}

bool Dialog::isOpen() const
{
    return background_ && background_->isVisible();
}

void Dialog::setPriority(OverlayManager::Priority priority)
{
    priority_ = priority;
    // Priority affects the next open(); an already attached overlay keeps its
    // position until it is closed and reopened.
}

void Dialog::setOnBackgroundClicked(const std::function<void()> &func)
{
    if (background_)
    {
        background_->setEnabled(true);
        background_->setOnClickedListener(func);
    }
}

void Dialog::setBackgroundOpacity(double opacity)
{
    if (background_)
    {
        background_->setOpacity(opacity);
    }
}

} // namespace lvglpp::widgets
