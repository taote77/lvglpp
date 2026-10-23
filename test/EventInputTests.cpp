#include "TestFramework.h"
#include "core/sys/InputManager.h"
#include "core/widgets/BaseDialog.h"
#include "core/widgets/Widget.h"
#include "core/widgets/KeyEvent.h"
#include "lvgl.h"

#include <memory>
#include <vector>

using namespace lvglpp::sys;
using namespace lvglpp::test;
using namespace lvglpp::widgets;

namespace {

class TestDialog : public BaseDialog
{
public:
    TestDialog() : BaseDialog(false)
    {}

    ~TestDialog() override = default;

    Widget *addFocusable()
    {
        auto item = std::make_unique<Widget>(getRoot());
        item->setFocusable(true);
        Widget *raw = item.get();
        items_.push_back(std::move(item));
        return raw;
    }

private:
    std::vector<std::unique_ptr<Widget>> items_;
};

lv_indev_data_t g_fake_key_data{};

void fakeKeypadRead(lv_indev_t *, lv_indev_data_t *data)
{
    *data = g_fake_key_data;
}

struct InputReset {
    InputReset()
    {
        InputManager::getInstance().shutdown();
        InputManager::getInstance().install();
    }

    ~InputReset()
    {
        InputManager::getInstance().shutdown();
    }
};

} // namespace

TEST(KeyEvent_maps_lvgl_keys)
{
    CHECK(keyFromLvgl(LV_KEY_UP) == Key::Up);
    CHECK(keyFromLvgl(LV_KEY_DOWN) == Key::Down);
    CHECK(keyFromLvgl(LV_KEY_LEFT) == Key::Left);
    CHECK(keyFromLvgl(LV_KEY_RIGHT) == Key::Right);
    CHECK(keyFromLvgl(LV_KEY_ENTER) == Key::Enter);
    CHECK(keyFromLvgl(LV_KEY_ESC) == Key::Escape);
    CHECK(keyFromLvgl(LV_KEY_NEXT) == Key::Next);
    CHECK(keyFromLvgl('A') == Key::Character);

    CHECK_EQ(lvglKey(Key::Enter), static_cast<uint32_t>(LV_KEY_ENTER));
    CHECK_EQ(lvglKey(Key::Escape), static_cast<uint32_t>(LV_KEY_ESC));
    CHECK_EQ(lvglKey(Key::Next), static_cast<uint32_t>(LV_KEY_NEXT));
    return true;
}

TEST(Input_scope_focuses_first_focusable)
{
    InputReset reset;
    auto       root = std::make_unique<Widget>();
    auto       a    = std::make_unique<Widget>(root.get());
    auto       b    = std::make_unique<Widget>(root.get());

    a->setFocusable(true);
    b->setFocusable(true);

    auto token = InputManager::getInstance().pushScope(root.get());

    CHECK(InputManager::getInstance().focusedItem() == a.get());
    CHECK(a->hasFocus());

    CHECK_TRUE(InputManager::getInstance().requestFocus(b.get()));
    CHECK(InputManager::getInstance().focusedItem() == b.get());
    CHECK_FALSE(a->hasFocus());
    CHECK(b->hasFocus());

    InputManager::getInstance().popScope(token);
    CHECK(InputManager::getInstance().focusedItem() == nullptr);
    return true;
}

TEST(Input_sink_when_scope_has_no_focusables)
{
    InputReset reset;
    auto       root = std::make_unique<Widget>();

    auto token = InputManager::getInstance().pushScope(root.get());
    CHECK(InputManager::getInstance().focusedItem() == root.get());

    InputManager::getInstance().popScope(token);
    return true;
}

TEST(Input_focus_sink_replaced_by_dynamic_focusable)
{
    InputReset reset;
    auto       root  = std::make_unique<Widget>();
    auto       token = InputManager::getInstance().pushScope(root.get());

    CHECK(InputManager::getInstance().focusedItem() == root.get()); // sink

    auto child = std::make_unique<Widget>(root.get());
    child->setFocusable(true);

    CHECK(InputManager::getInstance().focusedItem() == child.get());
    CHECK(child->hasFocus());

    InputManager::getInstance().popScope(token);
    return true;
}

TEST(Input_dynamic_focusable_registration)
{
    InputReset reset;
    auto       root  = std::make_unique<Widget>();
    auto       token = InputManager::getInstance().pushScope(root.get());

    auto child = std::make_unique<Widget>(root.get());
    child->setFocusable(true);
    CHECK_TRUE(InputManager::getInstance().requestFocus(child.get()));
    CHECK(child->hasFocus());

    child->setFocusable(false);
    CHECK_FALSE(InputManager::getInstance().requestFocus(child.get()));

    InputManager::getInstance().popScope(token);
    return true;
}

TEST(Input_key_bubbles_to_ancestor_then_global)
{
    InputReset reset;
    auto       root   = std::make_unique<Widget>();
    auto       parent = std::make_unique<Widget>(root.get());
    auto       child  = std::make_unique<Widget>(parent.get());

    child->setFocusable(true);

    int child_hits  = 0;
    int parent_hits = 0;
    int global_hits = 0;

    child->onKey([&child_hits](KeyEvent &event) {
        if (event.key == Key::Right)
        {
            child_hits++;
        }
        // not consumed: must bubble
    });

    parent->onKey([&parent_hits](KeyEvent &event) {
        if (event.key == Key::Right)
        {
            parent_hits++;
            event.consume();
        }
    });

    auto global_conn = InputManager::getInstance().addGlobalKeyHandler([&global_hits](KeyEvent &event) {
        global_hits++;
        event.consume();
    });

    auto token = InputManager::getInstance().pushScope(root.get());

    KeyEvent right{Key::Right, LV_KEY_RIGHT, KeyAction::Press, false};
    child->dispatchKeyEvent(right);
    CHECK_EQ(child_hits, 1);
    CHECK_EQ(parent_hits, 1);
    CHECK_EQ(global_hits, 0);
    CHECK_TRUE(right.consumed);

    KeyEvent left{Key::Left, LV_KEY_LEFT, KeyAction::Press, false};
    child->dispatchKeyEvent(left);
    CHECK_EQ(global_hits, 1);
    CHECK_TRUE(left.consumed);

    global_conn.disconnect();
    InputManager::getInstance().popScope(token);
    return true;
}

TEST(Input_global_handler_and_back_handler)
{
    InputReset reset;
    auto       root = std::make_unique<Widget>();

    int back_calls   = 0;
    int global_calls = 0;

    auto token = InputManager::getInstance().pushScope(root.get(), [&back_calls]() { back_calls++; });

    auto conn = InputManager::getInstance().addGlobalKeyHandler([&global_calls](KeyEvent &event) {
        if (event.key == Key::Escape)
        {
            global_calls++;
            event.consume();
        }
    });

    KeyEvent escape{Key::Escape, LV_KEY_ESC, KeyAction::Press, false};
    CHECK_TRUE(InputManager::getInstance().dispatchKey(escape));
    CHECK_EQ(global_calls, 1);
    // The global handler consumed the key, so the scope back handler must not run.
    CHECK_EQ(back_calls, 0);

    conn.disconnect();

    KeyEvent escape2{Key::Escape, LV_KEY_ESC, KeyAction::Press, false};
    CHECK_TRUE(InputManager::getInstance().dispatchKey(escape2));
    CHECK_EQ(back_calls, 1);

    int fallback_calls = 0;
    InputManager::getInstance().setBackHandler([&fallback_calls]() { fallback_calls++; });

    InputManager::getInstance().popScope(token);

    KeyEvent escape3{Key::Escape, LV_KEY_ESC, KeyAction::Press, false};
    CHECK_TRUE(InputManager::getInstance().dispatchKey(escape3));
    CHECK_EQ(fallback_calls, 1);

    InputManager::getInstance().setBackHandler(nullptr);
    return true;
}

TEST(Input_posted_key_reaches_focused_widget)
{
    InputReset reset;
    auto       root  = std::make_unique<Widget>();
    auto       child = std::make_unique<Widget>(root.get());
    child->setFocusable(true);

    int presses  = 0;
    int releases = 0;
    child->onKey([&presses, &releases](KeyEvent &event) {
        if (event.key == Key::Enter)
        {
            if (event.action == KeyAction::Press)
            {
                presses++;
            } else if (event.action == KeyAction::Release)
            {
                releases++;
            }
        }
    });

    auto token = InputManager::getInstance().pushScope(root.get());

    InputManager::getInstance().postKey(Key::Enter, true);
    lv_indev_read(InputManager::getInstance().virtualIndev());

    InputManager::getInstance().postKey(Key::Enter, false);
    lv_indev_read(InputManager::getInstance().virtualIndev());

    CHECK_EQ(presses, 1);
    CHECK_EQ(releases, 1);

    InputManager::getInstance().popScope(token);
    return true;
}

TEST(Input_lvgl_key_event_path)
{
    InputReset reset;
    auto       root  = std::make_unique<Widget>();
    auto       child = std::make_unique<Widget>(root.get());
    child->setFocusable(true);

    int presses  = 0;
    int releases = 0;
    child->onKey([&presses, &releases](KeyEvent &event) {
        if (event.key != Key::Enter)
        {
            return;
        }
        if (event.action == KeyAction::Press)
        {
            presses++;
        } else if (event.action == KeyAction::Release)
        {
            releases++;
        }
    });

    auto token = InputManager::getInstance().pushScope(root.get());

    // A regular LVGL keypad indev (like the SDL keyboard indev) must reach
    // onKey without being dispatched twice.
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev, fakeKeypadRead);
    lv_indev_set_display(indev, lv_display_get_default());
    lv_indev_set_group(indev, InputManager::getInstance().activeGroup());

    g_fake_key_data       = lv_indev_data_t{};
    g_fake_key_data.key   = LV_KEY_ENTER;
    g_fake_key_data.state = LV_INDEV_STATE_PRESSED;
    lv_indev_read(indev);

    g_fake_key_data.key   = LV_KEY_ENTER;
    g_fake_key_data.state = LV_INDEV_STATE_RELEASED;
    lv_indev_read(indev);

    CHECK_EQ(presses, 1);
    CHECK_EQ(releases, 1);

    lv_indev_delete(indev);
    InputManager::getInstance().popScope(token);
    return true;
}

TEST(Input_modal_scope_restores_previous_focus)
{
    InputReset reset;

    auto root1 = std::make_unique<Widget>();
    auto a     = std::make_unique<Widget>(root1.get());
    a->setFocusable(true);

    auto root2 = std::make_unique<Widget>();
    auto b     = std::make_unique<Widget>(root2.get());
    b->setFocusable(true);

    auto activity = InputManager::getInstance().pushScope(root1.get());
    CHECK(InputManager::getInstance().focusedItem() == a.get());

    auto modal = InputManager::getInstance().pushModalScope(root2.get());
    CHECK(InputManager::getInstance().focusedItem() == b.get());

    InputManager::getInstance().popScope(modal);
    CHECK(InputManager::getInstance().focusedItem() == a.get());

    InputManager::getInstance().popScope(activity);
    return true;
}

TEST(Input_dialog_modal_scope_closes_on_escape)
{
    InputReset reset;

    auto root           = std::make_unique<Widget>();
    auto activity_child = std::make_unique<Widget>(root.get());
    activity_child->setFocusable(true);

    auto activity_token = InputManager::getInstance().pushScope(root.get());
    CHECK(InputManager::getInstance().focusedItem() == activity_child.get());

    TestDialog dialog;
    Widget  *dialog_child = dialog.addFocusable();

    dialog.open();
    CHECK_TRUE(dialog.isOpen());
    CHECK(InputManager::getInstance().focusedItem() == dialog_child);

    KeyEvent escape{Key::Escape, LV_KEY_ESC, KeyAction::Press, false};
    CHECK_TRUE(InputManager::getInstance().dispatchKey(escape));

    CHECK_FALSE(dialog.isOpen());
    CHECK(InputManager::getInstance().focusedItem() == activity_child.get());

    InputManager::getInstance().popScope(activity_token);
    return true;
}

TEST(Input_focus_style_is_applied)
{
    InputReset reset;
    auto       root  = std::make_unique<Widget>();
    auto       child = std::make_unique<Widget>(root.get());

    child->setFocusable(true);
    auto token = InputManager::getInstance().pushScope(root.get());
    CHECK_TRUE(child->hasFocus());
    CHECK_EQ(lv_obj_get_style_outline_width(child->lvObj(), LV_PART_MAIN), 2);
    InputManager::getInstance().popScope(token);
    return true;
}
