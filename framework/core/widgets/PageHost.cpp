#include "PageHost.h"

#include "core/sys/InputManager.h"
#include "lvgl.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace lvglpp::widgets {

// ---------------------------------------------------------------- PageContext

void PageContext::setArguments(std::any args)
{
    arguments_ = std::move(args);
}

void PageContext::invokeArguments()
{
    if (arguments_handler_)
    {
        arguments_handler_(arguments_);
    }
}

void PageContext::invokeNewArguments()
{
    if (new_arguments_handler_)
    {
        new_arguments_handler_(arguments_);
    }
}

void PageContext::invokeShow()
{
    if (show_handler_)
    {
        show_handler_();
    }
}

void PageContext::invokeHide()
{
    if (hide_handler_)
    {
        hide_handler_();
    }
}

void PageContext::invokeResume()
{
    if (resume_handler_)
    {
        resume_handler_();
    }
}

void PageContext::invokePause()
{
    if (pause_handler_)
    {
        pause_handler_();
    }
}

void PageContext::invokeDestroy()
{
    if (destroy_handler_)
    {
        destroy_handler_();
    }
}

void PageContext::invokeResult(std::any result)
{
    if (result_handler_)
    {
        result_handler_(std::move(result));
    }
}

void PageContext::clearWidgets()
{
    widgets_.clear();
}

// ------------------------------------------------------------------- PageHost

struct PageHost::Page {
    std::string                  name;
    Builder                      builder;
    bool                         lazy           = true;
    bool                         root           = true;
    bool                         built          = false;
    bool                         ever_activated = false;
    std::uint32_t                last_shown     = 0;
    std::unique_ptr<Widget>      container;
    std::unique_ptr<PageContext> context;
};

PageHost::PageHost(Widget *parent) : Widget(parent)
{
    back_token_ = sys::InputManager::getInstance().addBackHandler(sys::BackPriority::Page, [this]() { return isEffectivelyVisible() && back(); });
}

PageHost::~PageHost()
{
    if (back_token_ != 0)
    {
        sys::InputManager::getInstance().removeBackHandler(back_token_);
        back_token_ = 0;
    }

    pending_ops_.clear();
    navigating_ = true;
    releaseAllNow();
    navigating_ = false;
}

int PageHost::addPage(std::string name, Builder builder, bool lazy, bool root)
{
    if (!builder)
    {
        return -1;
    }

    auto page     = std::make_unique<Page>();
    page->name    = std::move(name);
    page->builder = std::move(builder);
    page->lazy    = lazy;
    page->root    = root;

    const int index = static_cast<int>(pages_.size());
    pages_.push_back(std::move(page));
    stacks_.resize(pages_.size());

    if (!lazy)
    {
        ensurePage(index);
    }
    return index;
}

int PageHost::indexOf(const std::string &name) const
{
    for (std::size_t i = 0; i < pages_.size(); ++i)
    {
        if (pages_[i]->name == name)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::string PageHost::pageName(int index) const
{
    if (index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return {};
    }
    return pages_[index]->name;
}

Widget *PageHost::buildPage(int index, const std::any &args)
{
    if (index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return nullptr;
    }

    Page &page = *pages_[index];
    if (page.built)
    {
        return page.container.get();
    }

    page.container = std::make_unique<Widget>(this);
    page.container->setSizePercent(100, 100);
    page.container->setAlignment(LV_ALIGN_TOP_LEFT, 0, 0);
    page.container->setVisible(false);

    page.context = std::make_unique<PageContext>(page.container.get());
    page.context->setArguments(args);
    page.built = true;

    page.builder(*page.context);
    return page.container.get();
}

Widget *PageHost::ensurePage(int index)
{
    return buildPage(index, {});
}

void PageHost::activateEntry(const Entry &entry, const std::any *result, bool new_push)
{
    const int index = entry.page;
    if (index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return;
    }

    Page      &page      = *pages_[index];
    const bool was_built = page.built;
    Widget    *target    = buildPage(index, entry.args);
    if (target == nullptr || page.context == nullptr)
    {
        return;
    }

    page.context->setArguments(entry.args);
    if (!was_built || !page.ever_activated)
    {
        page.context->invokeArguments();
    } else if (new_push)
    {
        page.context->invokeNewArguments();
    }

    if (result != nullptr)
    {
        page.context->invokeResult(*result);
    }

    target->setVisible(true);
    current_            = index;
    page.last_shown     = lv_tick_get();
    page.ever_activated = true;

    page.context->invokeShow();
    page.context->invokeResume();
}

void PageHost::deactivateCurrent(bool destroy)
{
    if (current_ < 0 || current_ >= static_cast<int>(pages_.size()))
    {
        return;
    }

    const int index = current_;
    Page     &page  = *pages_[index];
    if (page.built && page.context)
    {
        page.context->invokePause();
        page.context->invokeHide();
        if (page.container && page.container->isAttached())
        {
            page.container->setVisible(false);
        }
    }

    current_ = -1;
    if (destroy)
    {
        destroyPage(index);
    }
}

void PageHost::destroyPage(int index)
{
    if (index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return;
    }

    Page &page = *pages_[index];
    if (!page.built)
    {
        return;
    }

    if (current_ == index)
    {
        current_ = -1;
    }

    if (page.context)
    {
        page.context->invokeDestroy();
        page.context->clearWidgets();
    }

    lv_obj_t *obj = page.container && page.container->isAttached() ? page.container->lvObj() : nullptr;
    if (obj != nullptr && lv_obj_is_valid(obj))
    {
        // Navigation may run from a widget callback inside this page; delete
        // the LVGL tree after the current event dispatch finishes.
        lv_obj_delete_async(obj);
    }

    page.container.reset();
    page.context.reset();
    page.built          = false;
    page.ever_activated = false;
}

void PageHost::queueOrRun(std::function<void()> op)
{
    if (navigating_)
    {
        pending_ops_.push_back(std::move(op));
        return;
    }

    navigating_ = true;
    op();
    navigating_ = false;
    processPendingOps();
}

void PageHost::processPendingOps()
{
    while (!navigating_ && !pending_ops_.empty())
    {
        auto op = std::move(pending_ops_.front());
        pending_ops_.pop_front();
        navigating_ = true;
        op();
        navigating_ = false;
    }
}

bool PageHost::doShow(int index)
{
    if (index < 0 || index >= static_cast<int>(pages_.size()) || !pages_[index]->root)
    {
        return false;
    }

    if (active_root_ == index)
    {
        if (depth() > 1)
        {
            return doPopTo(stacks_[index].front().page, {});
        }
        if (current_ == index)
        {
            return false;
        }
    }

    deactivateCurrent(false);

    active_root_ = index;
    if (stacks_[index].empty())
    {
        stacks_[index].push_back(Entry{index, {}});
    }

    activateEntry(stacks_[index].back(), nullptr, false);
    applyCachePolicy();
    refreshFocus();

    if (page_changed_)
    {
        page_changed_(current_, currentPage());
    }
    return true;
}

bool PageHost::show(int index)
{
    if (index < 0 || index >= static_cast<int>(pages_.size()) || !pages_[index]->root)
    {
        return false;
    }

    if (navigating_)
    {
        pending_ops_.push_back([this, index]() { doShow(index); });
        return true;
    }

    navigating_        = true;
    const bool changed = doShow(index);
    navigating_        = false;
    processPendingOps();
    return changed;
}

bool PageHost::show(const std::string &name)
{
    const int index = indexOf(name);
    return index >= 0 && show(index);
}

bool PageHost::inActiveStack(int pageIndex) const
{
    if (active_root_ < 0 || active_root_ >= static_cast<int>(stacks_.size()))
    {
        return false;
    }
    const auto &stack = stacks_[active_root_];
    return std::any_of(stack.begin(), stack.end(), [pageIndex](const Entry &entry) { return entry.page == pageIndex; });
}

bool PageHost::doPush(int index, std::any args)
{
    if (active_root_ < 0 || index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return false;
    }
    if (pages_[index]->root)
    {
        return false;
    }
    for (const auto &stack : stacks_)
    {
        if (std::any_of(stack.begin(), stack.end(), [index](const Entry &entry) { return entry.page == index; }))
        {
            return false;
        }
    }

    deactivateCurrent(false);
    stacks_[active_root_].push_back(Entry{index, std::move(args)});
    activateEntry(stacks_[active_root_].back(), nullptr, true);
    applyCachePolicy();
    refreshFocus();

    if (page_changed_)
    {
        page_changed_(current_, currentPage());
    }
    return true;
}

bool PageHost::push(int index, std::any args)
{
    if (active_root_ < 0 || index < 0 || index >= static_cast<int>(pages_.size()) || pages_[index]->root)
    {
        return false;
    }
    if (inActiveStack(index))
    {
        return false;
    }

    if (navigating_)
    {
        pending_ops_.push_back([this, index, args = std::move(args)]() mutable { doPush(index, std::move(args)); });
        return true;
    }

    navigating_        = true;
    const bool changed = doPush(index, std::move(args));
    navigating_        = false;
    processPendingOps();
    return changed;
}

bool PageHost::push(const std::string &name, std::any args)
{
    const int index = indexOf(name);
    return index >= 0 && push(index, std::move(args));
}

bool PageHost::doPop(std::any result)
{
    if (active_root_ < 0 || stacks_[active_root_].size() <= 1)
    {
        return false;
    }

    auto     &stack  = stacks_[active_root_];
    const int popped = stack.back().page;
    stack.pop_back();
    if (popped == current_)
    {
        deactivateCurrent(true);
    } else
    {
        destroyPage(popped);
    }

    activateEntry(stack.back(), &result, false);
    applyCachePolicy();
    refreshFocus();

    if (page_changed_)
    {
        page_changed_(current_, currentPage());
    }
    return true;
}

bool PageHost::pop(std::any result)
{
    if (active_root_ < 0 || stacks_[active_root_].size() <= 1)
    {
        return false;
    }

    if (navigating_)
    {
        pending_ops_.push_back([this, result = std::move(result)]() mutable { doPop(std::move(result)); });
        return true;
    }

    navigating_        = true;
    const bool changed = doPop(std::move(result));
    navigating_        = false;
    processPendingOps();
    return changed;
}

bool PageHost::doPopTo(int index, std::any result)
{
    if (active_root_ < 0)
    {
        return false;
    }

    auto &stack = stacks_[active_root_];
    if (stack.size() <= 1 || stack.back().page == index)
    {
        return false;
    }

    const bool found = std::any_of(stack.begin(), stack.end(), [index](const Entry &entry) { return entry.page == index; });
    if (!found)
    {
        return false;
    }

    while (!stack.empty() && stack.back().page != index)
    {
        const int popped = stack.back().page;
        stack.pop_back();

        if (popped == current_)
        {
            deactivateCurrent(true);
        } else
        {
            destroyPage(popped);
        }
    }

    if (stack.empty())
    {
        return false;
    }

    activateEntry(stack.back(), &result, false);
    applyCachePolicy();
    refreshFocus();

    if (page_changed_)
    {
        page_changed_(current_, currentPage());
    }
    return true;
}

bool PageHost::popTo(int index, std::any result)
{
    if (active_root_ < 0 || index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return false;
    }

    const auto &stack = stacks_[active_root_];
    if (stack.size() <= 1 || stack.back().page == index)
    {
        return false;
    }
    if (std::none_of(stack.begin(), stack.end(), [index](const Entry &entry) { return entry.page == index; }))
    {
        return false;
    }

    if (navigating_)
    {
        pending_ops_.push_back([this, index, result = std::move(result)]() mutable { doPopTo(index, std::move(result)); });
        return true;
    }

    navigating_        = true;
    const bool changed = doPopTo(index, std::move(result));
    navigating_        = false;
    processPendingOps();
    return changed;
}

bool PageHost::popToRoot()
{
    if (active_root_ < 0 || stacks_[active_root_].size() <= 1)
    {
        return false;
    }
    return popTo(stacks_[active_root_].front().page, {});
}

bool PageHost::back()
{
    return active_root_ >= 0 && stacks_[active_root_].size() > 1 ? pop() : false;
}

int PageHost::depth() const
{
    return active_root_ >= 0 && active_root_ < static_cast<int>(stacks_.size()) ? static_cast<int>(stacks_[active_root_].size()) : 0;
}

std::vector<int> PageHost::history() const
{
    std::vector<int> result;
    if (active_root_ < 0 || active_root_ >= static_cast<int>(stacks_.size()))
    {
        return result;
    }
    for (const Entry &entry : stacks_[active_root_])
    {
        result.push_back(entry.page);
    }
    return result;
}

Widget *PageHost::page(int index) const
{
    if (index < 0 || index >= static_cast<int>(pages_.size()) || !pages_[index]->built)
    {
        return nullptr;
    }
    return pages_[index]->container.get();
}

Widget *PageHost::currentPage() const
{
    return page(current_);
}

bool PageHost::isBuilt(int index) const
{
    return index >= 0 && index < static_cast<int>(pages_.size()) && pages_[index]->built;
}

void PageHost::release(int index)
{
    if (navigating_)
    {
        pending_ops_.push_back([this, index]() { release(index); });
        return;
    }

    navigating_            = true;
    const bool was_current = index == current_;
    if (was_current)
    {
        deactivateCurrent(true);
        refreshFocus();
    } else
    {
        destroyPage(index);
    }
    navigating_ = false;
    processPendingOps();
}

void PageHost::releaseAll()
{
    if (navigating_)
    {
        pending_ops_.push_back([this]() { releaseAll(); });
        return;
    }

    navigating_ = true;
    releaseAllNow();
    navigating_ = false;
    processPendingOps();
}

void PageHost::releaseAllNow()
{
    if (current_ >= 0 && current_ < static_cast<int>(pages_.size()))
    {
        const int current = current_;
        current_          = -1;
        Page &page        = *pages_[current];
        if (page.built && page.context)
        {
            page.context->invokePause();
            page.context->invokeHide();
        }
        destroyPage(current);
    }

    for (int i = 0; i < static_cast<int>(pages_.size()); ++i)
    {
        destroyPage(i);
    }
    for (auto &stack : stacks_)
    {
        stack.clear();
    }
    current_     = -1;
    active_root_ = -1;
}

void PageHost::setCachePolicy(CachePolicy policy, int maxCached)
{
    if (navigating_)
    {
        pending_ops_.push_back([this, policy, maxCached]() { setCachePolicy(policy, maxCached); });
        return;
    }

    policy_     = policy;
    max_cached_ = std::max(0, maxCached);

    navigating_ = true;
    applyCachePolicy();
    navigating_ = false;
    processPendingOps();
}

void PageHost::applyCachePolicy()
{
    if (policy_ == CachePolicy::KeepAll)
    {
        return;
    }

    if (policy_ == CachePolicy::ReleaseOnHide)
    {
        for (int i = 0; i < static_cast<int>(pages_.size()); ++i)
        {
            if (pages_[i]->built && !inActiveStack(i))
            {
                destroyPage(i);
            }
        }
        return;
    }

    // MaxCached: keep at most max_cached_ built pages outside the active stack.
    while (true)
    {
        int           victim      = -1;
        int           built_other = 0;
        std::uint32_t oldest      = std::numeric_limits<std::uint32_t>::max();

        for (int i = 0; i < static_cast<int>(pages_.size()); ++i)
        {
            Page &page = *pages_[i];
            if (!page.built || inActiveStack(i))
            {
                continue;
            }

            ++built_other;
            if (page.last_shown < oldest)
            {
                oldest = page.last_shown;
                victim = i;
            }
        }

        if (built_other <= max_cached_ || victim < 0)
        {
            break;
        }

        destroyPage(victim);
    }
}

void PageHost::refreshFocus()
{
    sys::InputManager::getInstance().refreshScopeContaining(this);
}

void PageHost::onLvglObjectDeleted()
{
    pending_ops_.clear();
    navigating_ = true;
    releaseAllNow();
    pending_ops_.clear();
    navigating_ = false;
}

PageHost::State PageHost::saveState() const
{
    State state;

    for (std::size_t root = 0; root < pages_.size(); ++root)
    {
        if (!pages_[root]->root || stacks_[root].empty())
        {
            continue;
        }

        std::vector<StateEntry> stack;
        stack.reserve(stacks_[root].size());
        for (const Entry &entry : stacks_[root])
        {
            if (entry.page < 0 || entry.page >= static_cast<int>(pages_.size()))
            {
                continue;
            }
            stack.push_back(StateEntry{pages_[entry.page]->name, entry.args});
        }

        if (!stack.empty())
        {
            state.stacks.push_back(std::move(stack));
        }
    }

    if (active_root_ >= 0 && active_root_ < static_cast<int>(pages_.size()))
    {
        state.activeRoot = pages_[active_root_]->name;
    }
    return state;
}

bool PageHost::restoreState(const State &state)
{
    if (state.stacks.empty())
    {
        return false;
    }

    // Validate all names before mutating the host.
    for (const auto &stack : state.stacks)
    {
        if (stack.empty())
        {
            return false;
        }
        const int root = indexOf(stack.front().name);
        if (root < 0 || !pages_[root]->root)
        {
            return false;
        }
        for (std::size_t i = 1; i < stack.size(); ++i)
        {
            const int index = indexOf(stack[i].name);
            if (index < 0 || pages_[index]->root)
            {
                return false;
            }
        }
    }

    navigating_ = true;
    pending_ops_.clear();
    releaseAllNow();
    pending_ops_.clear();
    navigating_ = false;

    int restore_root = -1;
    for (const auto &state_stack : state.stacks)
    {
        const int root = indexOf(state_stack.front().name);
        if (root < 0 || !pages_[root]->root)
        {
            continue;
        }

        std::vector<Entry> stack;
        stack.reserve(state_stack.size());
        stack.push_back(Entry{root, state_stack.front().args});
        for (std::size_t i = 1; i < state_stack.size(); ++i)
        {
            const int index = indexOf(state_stack[i].name);
            if (index >= 0 && !pages_[index]->root)
            {
                stack.push_back(Entry{index, state_stack[i].args});
            }
        }
        stacks_[root] = std::move(stack);

        if (restore_root < 0 || (!state.activeRoot.empty() && pages_[root]->name == state.activeRoot))
        {
            restore_root = root;
        }
    }

    if (restore_root < 0)
    {
        return false;
    }

    navigating_  = true;
    active_root_ = restore_root;
    activateEntry(stacks_[active_root_].back(), nullptr, false);
    navigating_ = false;
    applyCachePolicy();
    refreshFocus();
    processPendingOps();

    if (page_changed_)
    {
        page_changed_(current_, currentPage());
    }
    return true;
}

} // namespace lvglpp::widgets
