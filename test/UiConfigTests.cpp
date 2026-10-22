#include "TestFramework.h"
#include "core/sys/OrientationFilter.h"
#include "core/sys/UiConfig.h"
#include <lvgl.h>

using namespace lvglpp::test;
using namespace lvglpp::sys;

TEST(OrientationFilter_maps_four_directions)
{
    OrientationFilterConfig config;
    config.stable_ms   = 0;
    config.cooldown_ms = 0;
    OrientationFilter filter(config);

    CHECK_TRUE(!filter.update(1.0f, 0.0f, 0.0f).has_value()); // candidate
    CHECK_TRUE(filter.update(1.0f, 0.0f, 0.0f) == Orientation::Portrait);

    filter.setCurrentOrientation(Orientation::Portrait);
    CHECK_TRUE(!filter.update(-1.0f, 0.0f, 0.0f).has_value());
    CHECK_TRUE(filter.update(-1.0f, 0.0f, 0.0f) == Orientation::ReversePortrait);

    filter.setCurrentOrientation(Orientation::ReversePortrait);
    CHECK_TRUE(!filter.update(0.0f, 1.0f, 0.0f).has_value());
    CHECK_TRUE(filter.update(0.0f, 1.0f, 0.0f) == Orientation::Landscape);

    filter.setCurrentOrientation(Orientation::Landscape);
    CHECK_TRUE(!filter.update(0.0f, -1.0f, 0.0f).has_value());
    CHECK_TRUE(filter.update(0.0f, -1.0f, 0.0f) == Orientation::ReverseLandscape);
    return true;
}

TEST(OrientationFilter_keeps_flat_orientation)
{
    OrientationFilterConfig config;
    config.stable_ms = 0;
    OrientationFilter filter(config);

    filter.setCurrentOrientation(Orientation::Landscape);
    CHECK_TRUE(!filter.update(0.1f, 0.1f, 1.0f).has_value());
    CHECK_TRUE(!filter.update(0.0f, 0.0f, 1.0f).has_value());
    return true;
}

TEST(UiConfig_orientation_and_size)
{
    UiConfig::instance().shutdown();
    UiConfig::instance().initialize(nullptr);

    UiConfiguration cfg;
    cfg.orientation = Orientation::Portrait;
    cfg.direction   = LayoutDirection::Ltr;
    cfg.width       = 480;
    cfg.height      = 800;
    UiConfig::instance().applyNow(cfg, false);

    CHECK_TRUE(UiConfig::instance().width() == 480);
    CHECK_TRUE(UiConfig::instance().height() == 800);
    CHECK_TRUE(UiConfig::instance().isPortrait());
    CHECK_TRUE(!UiConfig::instance().isRtl());

    cfg.orientation = Orientation::ReverseLandscape;
    cfg.width       = 800;
    cfg.height      = 480;
    UiConfig::instance().applyNow(cfg, false);
    CHECK_TRUE(!UiConfig::instance().isPortrait());
    CHECK_TRUE(UiConfig::instance().current().isReverse());
    return true;
}

#if LVGLPP_ENABLE_RTL
TEST(UiConfig_direction_sets_layer_base_dir)
{
    UiConfig::instance().shutdown();
    UiConfig::instance().initialize(nullptr);

    UiConfiguration cfg;
    cfg.orientation = Orientation::Landscape;
    cfg.direction   = LayoutDirection::Rtl;
    cfg.width       = 800;
    cfg.height      = 480;
    UiConfig::instance().applyNow(cfg, false);

    CHECK_TRUE(UiConfig::instance().isRtl());
    CHECK_TRUE(lv_obj_get_style_base_dir(lv_screen_active(), LV_PART_MAIN) == LV_BASE_DIR_RTL);
    CHECK_TRUE(lv_obj_get_style_base_dir(lv_layer_top(), LV_PART_MAIN) == LV_BASE_DIR_RTL);

    cfg.direction = LayoutDirection::Ltr;
    UiConfig::instance().applyNow(cfg, false);
    CHECK_TRUE(lv_obj_get_style_base_dir(lv_screen_active(), LV_PART_MAIN) == LV_BASE_DIR_LTR);
    return true;
}
#endif
