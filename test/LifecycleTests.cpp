#include "TestFramework.h"
#include "core/widgets/Widget.h"
#include "core/widgets/GridLayout.h"
#include "core/widgets/WidgetStore.h"

#include <memory>
#include <type_traits>
#include <vector>

using namespace lvglpp::test;
using namespace lvglpp::widgets;

static_assert(!std::is_copy_constructible<Widget>::value, "Widget must not be copyable");
static_assert(!std::is_move_constructible<Widget>::value, "Widget must not be movable");

TEST(Widget_child_wrapper_can_die_while_object_lives)
{
    Widget parent;
    parent.setSize(200, 100);

    {
        auto child = std::make_unique<Widget>(&parent);
        child->setName("child");
        CHECK_TRUE(child->isAttached());
    }

    // The LVGL object is owned by the parent and still exists.
    CHECK_EQ(parent.childCount(), 1u);
    // ... but it no longer has a live wrapper.
    CHECK(parent.childAt(0) == nullptr);
    CHECK(parent.findChild("child") == nullptr);
    return true;
}

TEST(Widget_childAt_and_findChild_use_live_wrappers)
{
    Widget parent;
    auto     child = std::make_unique<Widget>(&parent);
    child->setName("live-child");
    child->setSize(10, 10);

    CHECK_EQ(parent.childCount(), 1u);
    CHECK(parent.childAt(0) == child.get());
    CHECK(parent.findChild("live-child") == child.get());
    CHECK(parent.findChildrenByType<Widget>().size() >= 2u);
    return true;
}

TEST(Widget_external_parent_delete_invalidates_wrappers)
{
    auto parent = std::make_unique<Widget>();
    auto child  = std::make_unique<Widget>(parent.get());

    CHECK_TRUE(parent->isAttached());
    CHECK_TRUE(child->isAttached());

    lv_obj_delete(parent->lvObj());

    CHECK_FALSE(parent->isAttached());
    CHECK_FALSE(child->isAttached());
    CHECK(child->lvObj() == nullptr);

    // Every operation must be a safe no-op on a detached wrapper.
    child->setSize(10, 10);
    child->setVisible(true);
    child->setBgColor(0x112233);
    child->dumpTree();
    CHECK_EQ(child->getWidth(), 0);
    CHECK_EQ(child->childCount(), 0u);
    return true;
}

TEST(Widget_owned_root_deletes_object)
{
    lv_obj_t      *screen = lv_scr_act();
    const uint32_t before = lv_obj_get_child_count(screen);

    {
        Widget root;
        CHECK_TRUE(root.ownsLvglObject());
        CHECK_EQ(lv_obj_get_child_count(screen), before + 1);
    }

    CHECK_EQ(lv_obj_get_child_count(screen), before);
    return true;
}

TEST(Widget_callbacks_removed_when_wrapper_dies)
{
    auto parent = std::make_unique<Widget>();
    int  calls  = 0;

    {
        auto child = std::make_unique<Widget>(parent.get());
        child->onClicked([&calls]() { calls++; });
        lv_obj_t *child_obj = child->lvObj();
        lv_obj_send_event(child_obj, LV_EVENT_CLICKED, nullptr);
        CHECK_EQ(calls, 1);
    }

    lv_obj_t *surviving_child = lv_obj_get_child(parent->lvObj(), 0);
    CHECK(surviving_child != nullptr);

    // The wrapper is gone; its event callbacks must have been removed, so this
    // must not touch freed memory.
    lv_obj_send_event(surviving_child, LV_EVENT_CLICKED, nullptr);
    lv_obj_send_event(surviving_child, LV_EVENT_PRESSED, nullptr);
    lv_obj_send_event(surviving_child, LV_EVENT_RELEASED, nullptr);
    CHECK_EQ(calls, 1);
    return true;
}

TEST(Widget_connection_disconnects_handler)
{
    auto parent = std::make_unique<Widget>();
    auto child  = std::make_unique<Widget>(parent.get());
    int  calls  = 0;

    auto conn = child->onClicked([&calls]() { calls++; });
    lv_obj_send_event(child->lvObj(), LV_EVENT_CLICKED, nullptr);
    CHECK_EQ(calls, 1);

    conn.disconnect();
    lv_obj_send_event(child->lvObj(), LV_EVENT_CLICKED, nullptr);
    CHECK_EQ(calls, 1);
    CHECK_FALSE(conn.connected());
    return true;
}

TEST(Widget_onDestroyed_fires_for_wrapper_destruction)
{
    int calls = 0;
    {
        auto item = std::make_unique<Widget>();
        item->onDestroyed([&calls]() { calls++; });
    }
    CHECK_EQ(calls, 1);
    return true;
}

TEST(Widget_onDestroyed_fires_for_external_delete)
{
    auto item  = std::make_unique<Widget>();
    int  calls = 0;
    item->onDestroyed([&calls]() { calls++; });

    lv_obj_delete(item->lvObj());
    CHECK_EQ(calls, 1);
    CHECK_FALSE(item->isAttached());
    return true;
}

TEST(Widget_setParent_updates_parent_and_ownership)
{
    Widget first;
    Widget second;
    auto     child = std::make_unique<Widget>(&first);

    child->setParent(&second);
    CHECK(child->getParent() == &second);
    CHECK_FALSE(child->ownsLvglObject());
    CHECK_EQ(second.childCount(), 1u);
    return true;
}

TEST(WidgetStore_keeps_wrappers_alive)
{
    Widget    parent;
    WidgetStore store;

    Widget *stored = store.emplace<Widget>(&parent);
    stored->setName("stored");
    CHECK_EQ(store.size(), 1u);
    CHECK(parent.findChild("stored") == stored);

    store.clear();
    CHECK_TRUE(store.empty());
    CHECK(parent.findChild("stored") == nullptr);
    // The lv_obj survives under the parent after the wrapper is gone.
    CHECK_EQ(parent.childCount(), 1u);
    return true;
}

TEST(Widget_grid_descriptor_parser)
{
    std::vector<lv_coord_t> tracks;
    CHECK_TRUE(GridLayout::parseTracks("1fr 2fr auto 100px content", tracks));
    CHECK_EQ(tracks.size(), 6u);
    CHECK_EQ(tracks[0], LV_GRID_FR(1));
    CHECK_EQ(tracks[1], LV_GRID_FR(2));
    CHECK_EQ(tracks[2], LV_GRID_CONTENT);
    CHECK_EQ(tracks[3], 100);
    CHECK_EQ(tracks[4], LV_GRID_CONTENT);
    CHECK_EQ(tracks[5], LV_GRID_TEMPLATE_LAST);

    CHECK_TRUE(GridLayout::parseTracks("", tracks));
    CHECK_EQ(tracks.size(), 1u);
    CHECK_EQ(tracks[0], LV_GRID_TEMPLATE_LAST);

    CHECK_FALSE(GridLayout::parseTracks("1fr nope", tracks));
    CHECK_FALSE(GridLayout::parseTracks("-5px", tracks));
    return true;
}

TEST(Widget_grid_two_axes_are_safe)
{
    Widget container;
    container.setSize(200, 100);

    // Setting only one axis must not leave LVGL with a null descriptor.
    container.setGridColumns("1fr 1fr");
    std::vector<std::unique_ptr<Widget>> children;
    for (int i = 0; i < 4; ++i)
    {
        auto child = std::make_unique<Widget>(&container);
        child->setGridCell(static_cast<uint32_t>(i % 2), static_cast<uint32_t>(i / 2));
        child->setSize(10, 10);
        children.push_back(std::move(child));
    }

    lv_obj_update_layout(container.lvObj());
    CHECK_EQ(container.childCount(), 4u);

    Widget *first  = container.childAt(0);
    Widget *second = container.childAt(1);
    CHECK(first != nullptr);
    CHECK(second != nullptr);
    if (first != nullptr && second != nullptr)
    {
        CHECK(second->getX() > first->getX());
    }
    return true;
}
