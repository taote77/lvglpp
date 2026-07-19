#include "TestFramework.h"
#include <cstdlib>
#include <lvgl.h>

int main()
{
    // Init LVGL in headless mode for widget tests
    lv_init();
    // Create a minimal display buffer for tests
    static lv_color_t buf[LV_HOR_RES_MAX * 10];
    static lv_display_t *disp = lv_display_create(800, 480);
    lv_display_set_buffers(disp, buf, nullptr, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    int failures = lvglpp::test::TestRunner::instance().runAll();

    lv_display_delete(disp);
    lv_deinit();
    return failures > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
