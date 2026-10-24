#include "TestFramework.h"
#include "core/base/LvObjFactory.h"
#include "core/sys/InputManager.h"
#include "core/sys/StackView.h"
#include "core/sys/UiConfig.h"
#include "core/widgets/Dialog.h"
#include "core/widgets/OverlayManager.h"
#include "core/widgets/Widget.h"
#include "lvgl.h"

#include <memory>

using namespace lvglpp::sys;
using namespace lvglpp::test;
using namespace lvglpp::widgets;

namespace {

class TestDialog : public Dialog
{
public:
    explicit TestDialog(Layer layer = Layer::Application) : Dialog(layer)
    {}

    Widget *rootWidget()
    {
        return getRoot();
    }
};

struct OverlayReset {
    OverlayReset()
    {
        OverlayManager::getInstance().shutdown();
        lvglpp::sys::StackView::getInstance().shutdown();
        lvglpp::sys::InputManager::getInstance().shutdown();
        lvglpp::sys::InputManager::getInstance().install();
    }

    ~OverlayReset()
    {
        OverlayManager::getInstance().shutdown();
        lvglpp::sys::InputManager::getInstance().shutdown();
    }
};

} // namespace

TEST(Overlay_dialogs_stack_lifo)
{
    OverlayReset reset;

    TestDialog first;
    TestDialog second;

    first.open();
    second.open();

    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Dialog), 2);
    CHECK(OverlayManager::getInstance().topPriority() == OverlayManager::Priority::Dialog);

    lv_obj_t *first_obj  = first.rootWidget()->lvObj();
    lv_obj_t *second_obj = second.rootWidget()->lvObj();
    CHECK(lv_obj_get_parent(first_obj) == lv_obj_get_parent(second_obj));
    CHECK(lv_obj_get_index(second_obj) > lv_obj_get_index(first_obj));

    CHECK_TRUE(lvglpp::sys::InputManager::getInstance().dispatchBack());
    CHECK_FALSE(second.isOpen());
    CHECK_TRUE(first.isOpen());
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Dialog), 1);

    CHECK_TRUE(lvglpp::sys::InputManager::getInstance().dispatchBack());
    CHECK_FALSE(first.isOpen());
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Dialog), 0);
    return true;
}

TEST(Overlay_system_dialog_outranks_application_dialog)
{
    OverlayReset reset;

    TestDialog application(Dialog::Layer::Application);
    TestDialog system(Dialog::Layer::System);

    // Open the application dialog last; priority must still put the system
    // dialog above it in both z-order and back handling.
    system.open();
    application.open();

    CHECK(OverlayManager::getInstance().topPriority() == OverlayManager::Priority::SystemDialog);
    CHECK(lv_obj_get_parent(system.rootWidget()->lvObj()) == lv_layer_sys());
    CHECK(lv_obj_get_parent(application.rootWidget()->lvObj()) == lv_layer_top());

    CHECK_TRUE(lvglpp::sys::InputManager::getInstance().dispatchBack());
    CHECK_FALSE(system.isOpen());
    CHECK_TRUE(application.isOpen());

    CHECK_TRUE(lvglpp::sys::InputManager::getInstance().dispatchBack());
    CHECK_FALSE(application.isOpen());
    return true;
}

TEST(Overlay_transient_is_not_in_back_chain)
{
    OverlayReset reset;

    auto transient = std::make_unique<Widget>(lvglpp::tools::LvObjFactory::createLvglDialog);
    transient->setVisible(true);
    const auto token = OverlayManager::getInstance().registerTransient(transient.get(), OverlayManager::Priority::Toast);

    CHECK_TRUE(static_cast<bool>(token));
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Toast), 1);
    CHECK_FALSE(lvglpp::sys::InputManager::getInstance().dispatchBack());

    TestDialog dialog;
    dialog.open();
    CHECK(OverlayManager::getInstance().topPriority() == OverlayManager::Priority::Dialog);
    CHECK_TRUE(lvglpp::sys::InputManager::getInstance().dispatchBack());
    CHECK_FALSE(dialog.isOpen());
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Toast), 1);

    OverlayManager::getInstance().unregisterTransient(token);
    transient.reset();
    return true;
}

#if LVGLPP_DYNAMIC_UI
TEST(Overlay_configuration_change_closes_dialogs)
{
    OverlayReset reset;

    TestDialog dialog;
    dialog.open();
    CHECK_TRUE(dialog.isOpen());

    lvglpp::sys::UiConfig::instance().emitSignal("configurationChanged");
    CHECK_FALSE(dialog.isOpen());
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Dialog), 0);
    return true;
}
#endif
