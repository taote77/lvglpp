#include "TestFramework.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/ImageButton.h"
#include "core/widgets/MsgBox.h"
#include "core/widgets/Scale.h"
#include "core/widgets/Spinner.h"

using namespace lvglpp::test;
using namespace lvglpp::widgets;

TEST(Spinner_creates_and_configures)
{
    BaseItem root;
    Spinner  spinner(&root);

    CHECK_TRUE(spinner.isAttached());
    spinner.setAnimParams(1500, 90);
    return true;
}

TEST(ImageButton_sources_and_state)
{
    BaseItem    root;
    ImageButton button(&root);

    CHECK_TRUE(button.isAttached());
    CHECK_TRUE(button.isFocusable());

    button.setSource(ImageButton::State::Released, "a.png", "b.png", "c.png");
    button.setSource(ImageButton::State::Pressed, "d.png", "e.png", "f.png");
    button.setState(ImageButton::State::Pressed);
    CHECK(button.state() == ImageButton::State::Pressed);
    return true;
}

TEST(Scale_configures_ticks_and_range)
{
    BaseItem root;
    Scale    scale(&root);

    CHECK_TRUE(scale.isAttached());
    scale.setRange(0, 100).setTotalTickCount(11).setMajorTickEvery(5).setLabelShow(true);
    scale.setMode(LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    return true;
}

TEST(MsgBox_builds_content_and_buttons)
{
    BaseItem root;
    MsgBox   box(&root);

    box.setTitle("Title");
    box.addText("Body");

    int       calls  = 0;
    BaseItem *button = box.addButton("OK", [&calls]() { calls++; });
    CHECK(button != nullptr);
    CHECK_TRUE(button->isFocusable());
    // title + text + footer container
    CHECK(box.childCount() >= 3u);

    // Clicking the native button must reach the wrapper callback and close.
    lv_obj_send_event(button->getLvglItem(), LV_EVENT_CLICKED, nullptr);
    CHECK_EQ(calls, 1);
    CHECK_FALSE(box.isAttached());
    return true;
}

TEST(MsgBox_button_callbacks_die_with_wrapper)
{
    BaseItem root;
    int      calls = 0;

    lv_obj_t *button_obj = nullptr;
    {
        MsgBox    box(&root);
        BaseItem *button = box.addButton("OK", [&calls]() { calls++; }, false);
        CHECK(button != nullptr);
        button_obj = button->getLvglItem();
    }

    // The MsgBox wrapper is gone; its button callbacks must have been removed.
    // The lv_obj is still owned by `root` (the message box was not closed).
    CHECK(button_obj != nullptr);
    lv_obj_send_event(button_obj, LV_EVENT_CLICKED, nullptr);
    CHECK_EQ(calls, 0);
    return true;
}
