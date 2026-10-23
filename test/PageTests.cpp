#include "TestFramework.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/InputManager.h"
#include "core/sys/StackView.h"
#include "core/widgets/Widget.h"
#include "core/widgets/BaseModel.hpp"
#include "core/widgets/PageHost.h"
#include "core/widgets/PageView.hpp"
#include "core/widgets/TabView.h"
#include "lvgl.h"

#include <algorithm>
#include <any>
#include <memory>
#include <string>
#include <vector>

using namespace lvglpp;
using namespace lvglpp::test;
using namespace lvglpp::sys;
using namespace lvglpp::widgets;

namespace {

class CountingActivity : public BaseActivity
{
public:
    explicit CountingActivity(std::string name) : name_(std::move(name))
    {}

    void onCreate(std::any arg) override
    {
        had_arg_ = arg.has_value();
        events_.push_back("create");
        BaseActivity::onCreate(arg);
    }

    void onResume() override
    {
        events_.push_back("resume");
        BaseActivity::onResume();
    }

    void onPause() override
    {
        events_.push_back("pause");
        BaseActivity::onPause();
    }

    void onDestroy() override
    {
        events_.push_back("destroy");
        BaseActivity::onDestroy();
    }

    void onNotifyUI(const Event &) override
    {}

    bool contains(const std::string &event) const
    {
        return std::find(events_.begin(), events_.end(), event) != events_.end();
    }

    std::size_t count(const std::string &event) const
    {
        return static_cast<std::size_t>(std::count(events_.begin(), events_.end(), event));
    }

    const std::string        name_;
    bool                     had_arg_ = false;
    std::vector<std::string> events_;
};

class TestPageDelegate : public PageViewDelegate<int>
{
public:
    explicit TestPageDelegate(Widget *parent = nullptr) : PageViewDelegate<int>(parent)
    {}

    void draw(const int &) override
    {
        setSize(10, 10);
    }
};

} // namespace

TEST(StackView_initializes_all_queued_views)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto first  = std::make_shared<CountingActivity>("first");
    auto second = std::make_shared<CountingActivity>("second");

    stack.pushView(first, 42);
    stack.pushView(second, std::string("arg"));
    stack.start();

    CHECK_TRUE(first->had_arg_);
    CHECK_TRUE(second->had_arg_);
    CHECK_EQ(first->count("create"), 1u);
    CHECK_EQ(first->count("pause"), 1u);
    CHECK_EQ(first->count("resume"), 0u);
    CHECK_EQ(second->count("create"), 1u);
    CHECK_EQ(second->count("resume"), 1u);

    stack.popViewImmediately();
    CHECK_EQ(second->count("pause"), 1u);
    CHECK_EQ(first->count("resume"), 1u);

    stack.clearDeleteVec();
    CHECK_EQ(second->count("destroy"), 1u);

    stack.shutdown();
    CHECK_EQ(first->count("pause"), 2u);
    CHECK_EQ(first->count("destroy"), 1u);
    return true;
}

TEST(StackView_pop_is_safe_at_root)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto only = std::make_shared<CountingActivity>("only");
    stack.pushView(only);
    stack.start();

    stack.popView();
    stack.popViewImmediately();
    CHECK_EQ(only->count("destroy"), 0u); // root cannot be popped

    stack.shutdown();
    CHECK_EQ(only->count("destroy"), 1u);
    return true;
}

TEST(BaseActivity_refresh_focus_skips_hidden_widgets)
{
    auto &input = InputManager::getInstance();
    input.shutdown();
    input.install();

    {
        CountingActivity activity("focus");
        activity.onCreate({});

        auto child = std::make_unique<Widget>(activity.getRoot());
        child->setFocusable(true);

        activity.onResume();
        CHECK(input.focusedItem() == child.get());

        child->setVisible(false);
        activity.refreshFocus();
        CHECK(input.focusedItem() != child.get());

        activity.onPause();
    }

    input.shutdown();
    return true;
}

TEST(PageView_destroy_cancels_pending_async_creation)
{
    BaseModel<int> model;
    for (int i = 0; i < 20; ++i)
    {
        model.append(i);
    }

    {
        auto page = std::make_unique<PageView<int, TestPageDelegate>>(model, nullptr);
        page->setSize(100, 100);
        page->update(); // schedules delegate creation through lv_async_call
    }

    // Run the async queue; a canceled callback must not touch the freed page.
    lv_timer_handler();
    return true;
}

TEST(PageHost_builds_pages_lazily)
{
    Widget root;
    PageHost host(&root);
    host.setSize(200, 100);

    int builds_a = 0;
    int builds_b = 0;

    host.addPage("A", [&builds_a](PageContext &ctx) {
        builds_a++;
        ctx.emplace<Widget>(ctx.root())->setName("a-child");
    });
    host.addPage("B", [&builds_b](PageContext &ctx) {
        builds_b++;
        ctx.emplace<Widget>(ctx.root())->setName("b-child");
    });

    CHECK_EQ(host.pageCount(), 2);
    CHECK_EQ(builds_a, 0);
    CHECK_EQ(builds_b, 0);
    CHECK_FALSE(host.isBuilt(0));
    CHECK(host.page(0) == nullptr);

    CHECK_TRUE(host.show(0));
    CHECK_EQ(builds_a, 1);
    CHECK_EQ(builds_b, 0);
    CHECK_EQ(host.currentIndex(), 0);
    CHECK_TRUE(host.isBuilt(0));
    CHECK_TRUE(host.page(0)->isVisible());

    CHECK_TRUE(host.show(1));
    CHECK_EQ(builds_b, 1);
    CHECK_FALSE(host.page(0)->isVisible());
    CHECK_TRUE(host.page(1)->isVisible());

    CHECK_TRUE(host.show(0));
    CHECK_EQ(builds_a, 1); // kept alive, not rebuilt
    CHECK_EQ(builds_b, 1);
    return true;
}

TEST(PageHost_runs_page_lifecycle)
{
    Widget root;
    PageHost host(&root);
    host.setSize(200, 100);

    int shown_a = 0, hidden_a = 0, destroyed_a = 0;
    int shown_b = 0, hidden_b = 0, destroyed_b = 0;

    host.addPage("A", [&](PageContext &ctx) { ctx.onShow([&shown_a]() { shown_a++; }).onHide([&hidden_a]() { hidden_a++; }).onDestroy([&destroyed_a]() { destroyed_a++; }); });
    host.addPage("B", [&](PageContext &ctx) { ctx.onShow([&shown_b]() { shown_b++; }).onHide([&hidden_b]() { hidden_b++; }).onDestroy([&destroyed_b]() { destroyed_b++; }); });

    host.show(0);
    CHECK_EQ(shown_a, 1);

    host.show(1);
    CHECK_EQ(hidden_a, 1);
    CHECK_EQ(shown_b, 1);

    host.release(0);
    CHECK_EQ(destroyed_a, 1);

    host.releaseAll();
    CHECK_EQ(hidden_b, 1);
    CHECK_EQ(destroyed_b, 1);
    return true;
}

TEST(PageHost_release_on_hide_rebuilds)
{
    Widget root;
    PageHost host(&root);
    host.setSize(200, 100);
    host.setCachePolicy(PageHost::CachePolicy::ReleaseOnHide);

    int builds_a    = 0;
    int destroyed_a = 0;

    host.addPage("A", [&](PageContext &ctx) {
        builds_a++;
        ctx.onDestroy([&destroyed_a]() { destroyed_a++; });
    });
    host.addPage("B", [](PageContext &) {});

    host.show(0);
    host.show(1);
    CHECK_FALSE(host.isBuilt(0));
    CHECK_EQ(destroyed_a, 1);

    host.show(0);
    CHECK_EQ(builds_a, 2);
    return true;
}

TEST(PageHost_max_cached_releases_oldest)
{
    Widget root;
    PageHost host(&root);
    host.setSize(200, 100);
    host.setCachePolicy(PageHost::CachePolicy::MaxCached, 1);

    host.addPage("A", [](PageContext &) {});
    host.addPage("B", [](PageContext &) {});
    host.addPage("C", [](PageContext &) {});

    host.show(0);
    host.show(1);
    CHECK_TRUE(host.isBuilt(0));
    CHECK_TRUE(host.isBuilt(1));

    host.show(2);
    CHECK_FALSE(host.isBuilt(0)); // least recently shown
    CHECK_TRUE(host.isBuilt(1));
    CHECK_TRUE(host.isBuilt(2));
    return true;
}

TEST(PageHost_switch_from_nav_click_is_safe)
{
    auto &input = InputManager::getInstance();
    input.shutdown();
    input.install();

    {
        CountingActivity activity("nav");
        activity.onCreate({});

        auto host = std::make_unique<PageHost>(activity.getRoot());
        host->setSize(400, 200);

        Widget *button_a = nullptr;
        Widget *button_b = nullptr;

        host->addPage("A", [&button_a](PageContext &ctx) {
            button_a = ctx.emplace<Widget>(ctx.root());
            button_a->setSize(10, 10);
            button_a->setFocusable(true);
        });
        host->addPage("B", [&button_b](PageContext &ctx) {
            button_b = ctx.emplace<Widget>(ctx.root());
            button_b->setSize(10, 10);
            button_b->setFocusable(true);
        });

        // A nav control living outside the PageHost, like smart_home's tabs.
        auto nav = std::make_unique<Widget>(activity.getRoot());
        nav->setSize(50, 20);
        nav->setFocusable(true);
        nav->onClicked([&host]() { host->show(1); });

        host->show(0);
        activity.onResume();
        CHECK(input.focusedItem() == button_a);

        // Focus refresh happens inside the clicked callback; this must not
        // corrupt LVGL's event dispatch.
        lv_obj_send_event(nav->lvObj(), LV_EVENT_CLICKED, nullptr);

        CHECK_EQ(host->currentIndex(), 1);
        CHECK(input.focusedItem() == button_b);

        activity.onPause();
    }

    input.shutdown();
    return true;
}

TEST(PageHost_destroys_pages_on_host_destruction)
{
    Widget root;
    int      destroyed = 0;

    {
        PageHost host(&root);
        host.setSize(200, 100);

        host.addPage("A", [&destroyed](PageContext &ctx) { ctx.onDestroy([&destroyed]() { destroyed++; }); });
        host.show(0);
    }

    CHECK_EQ(destroyed, 1);
    return true;
}

TEST(PageHost_refreshes_focus_after_switch)
{
    auto &input = InputManager::getInstance();
    input.shutdown();
    input.install();

    {
        CountingActivity activity("pages");
        activity.onCreate({});

        auto host = std::make_unique<PageHost>(activity.getRoot());
        host->setSize(200, 100);

        Widget *button_a = nullptr;
        Widget *button_b = nullptr;

        host->addPage("A", [&button_a](PageContext &ctx) {
            button_a = ctx.emplace<Widget>(ctx.root());
            button_a->setSize(10, 10);
            button_a->setFocusable(true);
        });
        host->addPage("B", [&button_b](PageContext &ctx) {
            button_b = ctx.emplace<Widget>(ctx.root());
            button_b->setSize(10, 10);
            button_b->setFocusable(true);
        });

        host->show(0);
        activity.onResume();
        CHECK(input.focusedItem() == button_a);

        host->show(1);
        CHECK(input.focusedItem() == button_b);

        activity.onPause();
    }

    input.shutdown();
    return true;
}

TEST(TabView_addTab_returns_owned_wrapper_and_content)
{
    TabView tab;
    tab.setSize(200, 200);

    auto page = tab.addTab("First");
    CHECK(page != nullptr);
    CHECK_TRUE(page->isAttached());
    CHECK(tab.content(0) == page->lvObj());
    CHECK(tab.content(5) == nullptr);

    auto second = tab.addTab("Second");
    CHECK(second != nullptr);
    CHECK(tab.content(1) == second->lvObj());
    return true;
}
