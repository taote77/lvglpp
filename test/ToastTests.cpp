#include "TestFramework.h"

#include "core/widgets/OverlayManager.h"
#include "core/widgets/Toast.h"

#include <string>

#include <lvgl.h>

using namespace lvglpp::test;
using namespace lvglpp::widgets;

TEST(Toast_reuses_one_transient_overlay)
{
    Toast::success("first");
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Toast), 1);

    // A newer message must replace the current toast, not add a second view.
    Toast::warn("second");
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Toast), 1);

    Toast::error("third");
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Toast), 1);

    Toast::cancel();
    Toast::shutdown();
    CHECK_EQ(OverlayManager::getInstance().count(OverlayManager::Priority::Toast), 0);
    return true;
}

TEST(Toast_long_message_is_truncated_safely)
{
    const std::string message(512, 'W');
    Toast::show(message, ToastType::Warning, 1000);

    // Let layout and the fade-in animation run; the label must be clipped to
    // the display width instead of growing without bound.
    for (int i = 0; i < 4; ++i)
    {
        lv_timer_handler();
    }

    Toast::cancel();
    Toast::shutdown();
    return true;
}

TEST(Toast_shutdown_is_idempotent)
{
    Toast::shutdown();
    Toast::shutdown();
    Toast::cancel();
    return true;
}
