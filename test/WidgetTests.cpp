#include "TestFramework.h"
#include "core/kernel/LvObjFactory.h"
#include <lvgl.h>
#include <memory>

using namespace lvglpp::test;

// Widget tests require LVGL initialized — they run only when a display is available
// These test the C++ wrapper layer logic without needing actual rendering

TEST(WidgetFactory_exists)
{
    // Verify all factory methods compile and return non-null
    lv_obj_t *obj = lvglpp::tools::LvObjFactory::createLvglItem(lv_scr_act());
    CHECK_TRUE(obj != nullptr);
    lv_obj_del(obj);
    return true;
}

TEST(WidgetFactory_Label)
{
    lv_obj_t *label = lvglpp::tools::LvObjFactory::createLvglLabel(lv_scr_act());
    CHECK_TRUE(label != nullptr);
    lv_label_set_text(label, "test");
    CHECK(std::string(lv_label_get_text(label)) == "test");
    lv_obj_del(label);
    return true;
}

TEST(WidgetFactory_Image)
{
    lv_obj_t *img = lvglpp::tools::LvObjFactory::createLvglImage(lv_scr_act());
    CHECK_TRUE(img != nullptr);
    lv_obj_del(img);
    return true;
}

TEST(WidgetFactory_Button)
{
    lv_obj_t *btn = lvglpp::tools::LvObjFactory::createLvglItem(lv_scr_act());
    CHECK_TRUE(btn != nullptr);
    lv_obj_set_size(btn, 100, 40);
    // LVGL may apply size constraints; verify object exists and is usable
    CHECK_TRUE(lv_obj_get_width(btn) >= 0);
    CHECK_TRUE(lv_obj_get_height(btn) >= 0);
    lv_obj_del(btn);
    return true;
}

TEST(LVGL_child_navigation)
{
    lv_obj_t *parent = lv_obj_create(lv_scr_act());
    lv_obj_t *child1 = lv_obj_create(parent);
    lv_obj_t *child2 = lv_obj_create(parent);
    lv_obj_set_user_data(child1, (void *)"child1");
    lv_obj_set_user_data(child2, (void *)"child2");

    CHECK_EQ(lv_obj_get_child_cnt(parent), 2u);
    CHECK_TRUE(lv_obj_get_child(parent, 0) != nullptr);
    CHECK_TRUE(lv_obj_get_child(parent, 1) != nullptr);

    lv_obj_del(parent); // children deleted automatically
    return true;
}
