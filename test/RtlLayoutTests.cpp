#if LVGLPP_ENABLE_RTL

#include "TestFramework.h"
#include "core/sys/UiConfig.h"
#include "core/widgets/BaseItem.h"
#include <lvgl.h>

using namespace lvglpp::test;
using namespace lvglpp::widgets;
using namespace lvglpp::sys;

namespace {

void setDirection(LayoutDirection direction)
{
    UiConfiguration cfg = UiConfig::instance().current();
    cfg.direction       = direction;
    UiConfig::instance().applyNow(cfg, false);
}

} // namespace

TEST(RtlLayout_alignment_mirrors)
{
    UiConfig::instance().shutdown();
    UiConfig::instance().initialize(nullptr);
    setDirection(LayoutDirection::Ltr);

    BaseItem screen(BaseItem::AdoptExistingTag::Adopt, lv_screen_active());
    BaseItem parent(&screen);
    parent.setSize(400, 300);

    BaseItem child(&parent);
    child.setSize(100, 50);
    child.setAlignment(LV_ALIGN_TOP_LEFT, 10, 5);
    lv_obj_update_layout(parent.getLvglItem());
    CHECK_TRUE(child.getX() == 10);

    setDirection(LayoutDirection::Rtl);
    child.setAlignment(LV_ALIGN_TOP_LEFT, 10, 5);
    lv_obj_update_layout(parent.getLvglItem());
    CHECK_TRUE(child.getX() == 290); // 400 - 100 - 10

    child.setAlignment(LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_update_layout(parent.getLvglItem());
    CHECK_TRUE(child.getX() == 10); // mirrored to LEFT_MID +10

    child.setAlignment(LV_ALIGN_TOP_MID, 20, 0);
    lv_obj_update_layout(parent.getLvglItem());
    CHECK_TRUE(child.getX() == 130); // 150 - 20

    child.setPos(10, 5);
    lv_obj_update_layout(parent.getLvglItem());
    CHECK_TRUE(child.getX() == 290);

    setDirection(LayoutDirection::Ltr);
    return true;
}

TEST(RtlLayout_mirror_can_be_disabled)
{
    UiConfig::instance().shutdown();
    UiConfig::instance().initialize(nullptr);
    setDirection(LayoutDirection::Rtl);

    BaseItem screen(BaseItem::AdoptExistingTag::Adopt, lv_screen_active());
    BaseItem parent(&screen);
    parent.setSize(400, 300);

    BaseItem child(&parent);
    child.setSize(100, 50);
    child.setMirrorInRtl(false);
    child.setAlignment(LV_ALIGN_TOP_LEFT, 10, 5);
    lv_obj_update_layout(parent.getLvglItem());
    CHECK_TRUE(child.getX() == 10);

    setDirection(LayoutDirection::Ltr);
    return true;
}

TEST(RtlLayout_alignment_to_mirrors)
{
    UiConfig::instance().shutdown();
    UiConfig::instance().initialize(nullptr);
    setDirection(LayoutDirection::Rtl);

    BaseItem screen(BaseItem::AdoptExistingTag::Adopt, lv_screen_active());
    BaseItem parent(&screen);
    parent.setSize(400, 300);

    BaseItem anchor(&parent);
    anchor.setSize(100, 50);
    anchor.setAlignmentPhysical(LV_ALIGN_TOP_LEFT, 50, 50);

    BaseItem child(&parent);
    child.setSize(40, 20);
    child.setAlignmentTo(anchor, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_update_layout(parent.getLvglItem());
    // RTL: OUT_RIGHT_MID -> OUT_LEFT_MID with -5, so the child sits left of
    // the anchor (anchor x=50, child width=40, offset 5 -> x=5).
    CHECK_TRUE(child.getX() == 5);

    setDirection(LayoutDirection::Ltr);
    return true;
}

#endif // LVGLPP_ENABLE_RTL
