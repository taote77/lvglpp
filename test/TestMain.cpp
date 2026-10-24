#include "TestFramework.h"
#include "core/sys/InputManager.h"
#include "core/tools/AssetManager.h"
#include "core/tools/ImageManager.h"
#include "core/widgets/OverlayManager.h"
#include <cstdlib>
#include <lvgl.h>

namespace {

void flush_cb(lv_display_t *disp, const lv_area_t *, uint8_t *)
{
    lv_display_flush_ready(disp);
}

} // namespace

int main()
{
    // Init LVGL in headless mode for widget tests
    lv_init();
    // Create a minimal display buffer for tests
    static lv_color_t    buf[LV_HOR_RES_MAX * 10];
    static lv_display_t *disp = lv_display_create(800, 480);
    lv_display_set_buffers(disp, buf, nullptr, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Without a flush callback LVGL's refresh timer waits forever in
    // wait_for_flushing(). Headless tests just acknowledge each flush.
    lv_display_set_flush_cb(disp, flush_cb);

    int failures = lvglpp::test::TestRunner::instance().runAll();

    lvglpp::tools::ImageManager::shutdown();
    lvglpp::tools::AssetManager::getInstance().reset();
    lvglpp::widgets::OverlayManager::getInstance().shutdown();
    lvglpp::sys::InputManager::getInstance().shutdown();

    lv_display_delete(disp);
    lv_deinit();
    return failures > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
