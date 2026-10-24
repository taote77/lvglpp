#include "TestFramework.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/InputManager.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseModel.hpp"
#include "core/widgets/PageHost.h"
#include "core/widgets/PageView.hpp"
#include "core/widgets/TabView.h"
#include "core/widgets/Widget.h"
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

class RouteActivity : public CountingActivity
{
public:
    explicit RouteActivity(std::string name) : CountingActivity(std::move(name))
    {}

    std::string routeId() const override
    {
        return name_;
    }

    void onNewArguments(std::any arg) override
    {
        (void)arg;
        new_arguments++;
    }

    bool onBackPressed() override
    {
        back_calls++;
        return consume_back;
    }

    int  new_arguments = 0;
    int  back_calls    = 0;
    bool consume_back  = false;
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

TEST(StackView_delivers_result_to_pusher)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto first  = std::make_shared<RouteActivity>("first");
    auto second = std::make_shared<RouteActivity>("second");

    stack.pushView(first);
    stack.start();

    int      result_calls = 0;
    std::any result;
    CHECK_TRUE(stack.pushViewImmediately(second, 5, [&](std::any value) {
        result_calls++;
        result = std::move(value);
    }));
    CHECK_EQ(stack.depth(), 2);

    CHECK_TRUE(stack.popViewImmediately(42));
    CHECK_EQ(result_calls, 1);
    CHECK_EQ(std::any_cast<int>(result), 42);
    CHECK_EQ(first->count("resume"), 2u);

    stack.shutdown();
    return true;
}

TEST(StackView_single_top_reuses_instance)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto activity = std::make_shared<RouteActivity>("route");

    stack.pushView(activity);
    stack.start();

    CHECK_TRUE(stack.pushViewImmediately(activity, {}, {}, StackView::LaunchMode::SingleTop));
    CHECK_EQ(stack.depth(), 1);
    CHECK_EQ(activity->new_arguments, 1);

    stack.shutdown();
    return true;
}

TEST(StackView_clear_top_pops_to_matching_route)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto a = std::make_shared<RouteActivity>("a");
    auto b = std::make_shared<RouteActivity>("b");
    auto c = std::make_shared<RouteActivity>("c");

    stack.pushView(a);
    stack.start();
    stack.pushViewImmediately(b);
    stack.pushViewImmediately(c);
    CHECK_EQ(stack.depth(), 3);

    CHECK_TRUE(stack.pushViewImmediately(a, {}, {}, StackView::LaunchMode::ClearTop));
    CHECK_EQ(stack.depth(), 1);
    CHECK_EQ(a->new_arguments, 1);
    CHECK_EQ(b->count("destroy"), 0u); // async delete, but paused at least
    CHECK_EQ(b->count("pause"), 1u);
    CHECK_EQ(c->count("pause"), 1u);

    stack.clearDeleteVec();
    stack.shutdown();
    return true;
}

TEST(StackView_pop_to_and_back_handler)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto a = std::make_shared<RouteActivity>("a");
    auto b = std::make_shared<RouteActivity>("b");
    auto c = std::make_shared<RouteActivity>("c");

    stack.pushView(a);
    stack.start();
    stack.pushViewImmediately(b);
    stack.pushViewImmediately(c);

    CHECK_TRUE(stack.popTo(2));
    CHECK_EQ(stack.depth(), 2);

    CHECK_TRUE(InputManager::getInstance().dispatchBack());
    CHECK_EQ(a->back_calls, 0); // current is b
    CHECK_EQ(b->back_calls, 1);

    // Back pop is animated; drain the transition.
    for (int i = 0; i < 10; ++i)
    {
        lv_tick_inc(40);
        lv_timer_handler();
    }
    CHECK_EQ(stack.depth(), 1);

    a->consume_back = true;
    CHECK_TRUE(InputManager::getInstance().dispatchBack());
    CHECK_EQ(a->back_calls, 1);
    CHECK_EQ(stack.depth(), 1);

    stack.shutdown();
    return true;
}

TEST(StackView_replace_replaces_only_the_top)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto a = std::make_shared<RouteActivity>("a");
    auto b = std::make_shared<RouteActivity>("b");

    stack.pushView(a);
    stack.start();
    CHECK_TRUE(stack.pushViewAndReplacedImmediately(b));
    CHECK_EQ(stack.depth(), 1);
    CHECK_EQ(a->count("pause"), 1u);
    CHECK_EQ(b->count("resume"), 1u);

    stack.clearDeleteVec();
    CHECK_EQ(a->count("destroy"), 1u);

    stack.shutdown();
    return true;
}

TEST(StackView_queues_navigation_during_enter_animation)
{
    auto &stack = StackView::getInstance();
    stack.shutdown();

    auto a = std::make_shared<RouteActivity>("a");
    auto b = std::make_shared<RouteActivity>("b");
    auto c = std::make_shared<RouteActivity>("c");

    stack.pushView(a);
    stack.start();
    stack.pushView(b);
    CHECK_TRUE(stack.isAnimating());
    CHECK_TRUE(stack.pushView(c));
    CHECK_EQ(stack.depth(), 2); // c is queued behind b's enter animation

    for (int i = 0; i < 10; ++i)
    {
        lv_tick_inc(40);
        lv_timer_handler();
    }
    CHECK_EQ(stack.depth(), 3);

    stack.shutdown();
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
    Widget   root;
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
    Widget   root;
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
    Widget   root;
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
    Widget   root;
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
    int    destroyed = 0;

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

TEST(PageHost_push_pop_arguments_and_results)
{
    Widget   root;
    PageHost host(&root);
    host.setSize(200, 100);

    int received_args   = 0;
    int received_result = -1;
    int resumed         = 0;
    int paused          = 0;

    const int root_page = host.addPage("Root", [&](PageContext &ctx) { ctx.onResult([&](std::any result) { received_result = std::any_cast<int>(result); }); });
    const int child     = host.addPage(
        "Child",
        [&](PageContext &ctx) {
            ctx.onArguments([&](const std::any &args) { received_args = std::any_cast<int>(args); }).onResume([&]() { resumed++; }).onPause([&]() { paused++; });
        },
        true, false);

    CHECK_TRUE(host.show(root_page));
    CHECK_TRUE(host.push(child, 7));
    CHECK_EQ(host.depth(), 2);
    CHECK_EQ(host.currentIndex(), child);
    CHECK_EQ(received_args, 7);
    CHECK_EQ(host.history().size(), std::size_t{2});

    CHECK_TRUE(host.pop(99));
    CHECK_EQ(host.currentIndex(), root_page);
    CHECK_EQ(received_result, 99);
    CHECK_EQ(paused, 1);
    CHECK_EQ(resumed, 1);

    CHECK_FALSE(host.pop());
    return true;
}

TEST(PageHost_lifecycle_order_on_push_and_pop)
{
    Widget                   root;
    std::vector<std::string> events;
    PageHost                 host(&root);
    host.setSize(200, 100);

    const int tab   = host.addPage("Tab", [&](PageContext &ctx) {
        ctx.onShow([&]() { events.push_back("tab.show"); })
            .onResume([&]() { events.push_back("tab.resume"); })
            .onPause([&]() { events.push_back("tab.pause"); })
            .onHide([&]() { events.push_back("tab.hide"); })
            .onResult([&](std::any) { events.push_back("tab.result"); });
    });
    const int child = host.addPage(
        "Child",
        [&](PageContext &ctx) {
            ctx.onArguments([&](const std::any &) { events.push_back("child.args"); })
                .onShow([&]() { events.push_back("child.show"); })
                .onResume([&]() { events.push_back("child.resume"); })
                .onPause([&]() { events.push_back("child.pause"); })
                .onHide([&]() { events.push_back("child.hide"); })
                .onDestroy([&]() { events.push_back("child.destroy"); });
        },
        true, false);

    events.clear();
    host.show(tab);
    CHECK(events == std::vector<std::string>({"tab.show", "tab.resume"}));

    events.clear();
    host.push(child);
    CHECK(events == std::vector<std::string>({"tab.pause", "tab.hide", "child.args", "child.show", "child.resume"}));

    events.clear();
    host.pop(7);
    CHECK(events == std::vector<std::string>({"child.pause", "child.hide", "child.destroy", "tab.result", "tab.show", "tab.resume"}));
    return true;
}

TEST(PageHost_keeps_independent_root_histories)
{
    Widget   root;
    PageHost host(&root);
    host.setSize(200, 100);

    const int a = host.addPage("A", [](PageContext &) {});
    const int b = host.addPage("B", [](PageContext &) {});
    const int c = host.addPage("C", [](PageContext &) {}, true, false);
    const int d = host.addPage("D", [](PageContext &) {}, true, false);

    host.show(a);
    CHECK_TRUE(host.push(c));
    CHECK_EQ(host.currentIndex(), c);

    host.show(b);
    CHECK_TRUE(host.push(d));
    CHECK_EQ(host.currentIndex(), d);

    host.show(a);
    CHECK_EQ(host.currentIndex(), c);
    CHECK_EQ(host.depth(), 2);

    host.show(b);
    CHECK_EQ(host.currentIndex(), d);

    // Reselecting the active root pops it back to the root page.
    CHECK_TRUE(host.show(b));
    CHECK_EQ(host.currentIndex(), b);
    CHECK_EQ(host.depth(), 1);
    return true;
}

TEST(PageHost_rejects_invalid_navigation)
{
    Widget   root;
    PageHost host(&root);

    const int tab   = host.addPage("Tab", [](PageContext &) {});
    const int child = host.addPage("Child", [](PageContext &) {}, true, false);

    CHECK_FALSE(host.show(child)); // subpages cannot be selected as tabs
    CHECK_FALSE(host.push(tab));   // root pages cannot be pushed
    CHECK_FALSE(host.push(child)); // no active root yet
    CHECK_FALSE(host.pop());

    host.show(tab);
    CHECK_TRUE(host.push(child));
    CHECK_FALSE(host.push(child)); // a page may only be in one stack
    return true;
}

TEST(PageHost_pop_to_destroys_skipped_pages)
{
    Widget   root;
    PageHost host(&root);
    host.setSize(200, 100);

    int       destroyed = 0;
    const int tab       = host.addPage("Tab", [](PageContext &) {});
    const int one       = host.addPage("One", [&](PageContext &ctx) { ctx.onDestroy([&]() { destroyed++; }); }, true, false);
    const int two       = host.addPage("Two", [&](PageContext &ctx) { ctx.onDestroy([&]() { destroyed++; }); }, true, false);

    host.show(tab);
    host.push(one);
    host.push(two);
    CHECK_EQ(host.depth(), 3);

    CHECK_TRUE(host.popTo(one));
    CHECK_EQ(host.currentIndex(), one);
    CHECK_EQ(host.depth(), 2);
    CHECK_EQ(destroyed, 1);
    return true;
}

TEST(PageHost_save_and_restore_nested_stack)
{
    Widget   root1;
    PageHost first(&root1);
    first.setSize(200, 100);

    int args = 0;
    first.addPage("A", [](PageContext &) {});
    first.addPage("B", [](PageContext &) {});
    const int child = first.addPage("Child", [&](PageContext &ctx) { ctx.onArguments([&](const std::any &value) { args = std::any_cast<int>(value); }); }, true, false);

    first.show(1);
    first.push(child, 33);
    CHECK_EQ(args, 33);

    const PageHost::State state = first.saveState();
    CHECK_EQ(state.stacks.size(), std::size_t{1});
    CHECK_EQ(state.activeRoot, std::string("B"));

    Widget   root2;
    PageHost second(&root2);
    second.setSize(200, 100);
    second.addPage("A", [](PageContext &) {});
    second.addPage("B", [](PageContext &) {});
    const int second_child = second.addPage("Child", [&](PageContext &ctx) { ctx.onArguments([&](const std::any &value) { args = std::any_cast<int>(value); }); }, true, false);

    CHECK_TRUE(second.restoreState(state));
    CHECK_EQ(second.currentRoot(), 1);
    CHECK_EQ(second.currentIndex(), second_child);
    CHECK_EQ(second.depth(), 2);
    CHECK_EQ(args, 33);
    return true;
}

TEST(PageHost_back_handler_pops_only_visible_host)
{
    InputManager::getInstance().shutdown();
    InputManager::getInstance().install();
    StackView::getInstance().shutdown();

    Widget   parent;
    PageHost host(&parent);
    host.setSize(200, 100);
    const int tab   = host.addPage("Tab", [](PageContext &) {});
    const int child = host.addPage("Child", [](PageContext &) {}, true, false);

    host.show(tab);
    host.push(child);

    CHECK_TRUE(InputManager::getInstance().dispatchBack());
    CHECK_EQ(host.depth(), 1);

    host.setVisible(false);
    CHECK_FALSE(InputManager::getInstance().dispatchBack());

    InputManager::getInstance().shutdown();
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
