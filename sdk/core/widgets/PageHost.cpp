#include "PageHost.h"

#include "core/sys/InputManager.h"
#include "lvgl.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace lvglpp::widgets {

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

void PageContext::invokeDestroy()
{
    if (destroy_handler_)
    {
        destroy_handler_();
    }
}

void PageContext::clearWidgets()
{
    widgets_.clear();
}

struct PageHost::Page {
    std::string                  name;
    Builder                      builder;
    bool                         lazy       = true;
    bool                         built      = false;
    std::uint32_t                last_shown = 0;
    std::unique_ptr<BaseItem>    container;
    std::unique_ptr<PageContext> context;
};

PageHost::PageHost(BaseItem *parent) : BaseItem(parent)
{}

PageHost::~PageHost()
{
    releaseAll();
}

int PageHost::addPage(std::string name, Builder builder, bool lazy)
{
    if (!builder)
    {
        return -1;
    }

    auto page     = std::make_unique<Page>();
    page->name    = std::move(name);
    page->builder = std::move(builder);
    page->lazy    = lazy;

    const int index = static_cast<int>(pages_.size());
    pages_.push_back(std::move(page));

    if (!lazy)
    {
        ensurePage(index);
    }
    return index;
}

std::string PageHost::pageName(int index) const
{
    if (index < 0 || index >= static_cast<int>(pages_.size()))
    {
        return {};
    }
    return pages_[index]->name;
}

BaseItem *PageHost::buildPage(int index)
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

    page.container = std::make_unique<BaseItem>(this);
    page.container->setSizePercent(100, 100);
    page.container->setAlignment(LV_ALIGN_TOP_LEFT, 0, 0);
    page.container->setVisible(false);

    page.context = std::make_unique<PageContext>(page.container.get());
    page.built   = true;

    page.builder(*page.context);
    return page.container.get();
}

BaseItem *PageHost::ensurePage(int index)
{
    return buildPage(index);
}

bool PageHost::show(int index)
{
    if (index < 0 || index >= static_cast<int>(pages_.size()) || index == current_)
    {
        return false;
    }

    const int previous = current_;

    if (previous >= 0 && previous < static_cast<int>(pages_.size()))
    {
        Page &page = *pages_[previous];
        if (page.built)
        {
            if (page.context)
            {
                page.context->invokeHide();
            }
            if (page.container && page.container->isAttached())
            {
                page.container->setVisible(false);
            }
        }
    }

    current_ = -1;

    if (previous >= 0 && policy_ == CachePolicy::ReleaseOnHide)
    {
        destroyPage(previous);
    }

    BaseItem *target = buildPage(index);
    if (target == nullptr)
    {
        return false;
    }

    target->setVisible(true);
    pages_[index]->last_shown = lv_tick_get();
    if (pages_[index]->context)
    {
        pages_[index]->context->invokeShow();
    }

    current_ = index;
    applyCachePolicy();
    refreshFocus();

    if (page_changed_)
    {
        page_changed_(index, target);
    }
    return true;
}

BaseItem *PageHost::page(int index) const
{
    if (index < 0 || index >= static_cast<int>(pages_.size()) || !pages_[index]->built)
    {
        return nullptr;
    }
    return pages_[index]->container.get();
}

BaseItem *PageHost::currentPage() const
{
    return page(current_);
}

bool PageHost::isBuilt(int index) const
{
    return index >= 0 && index < static_cast<int>(pages_.size()) && pages_[index]->built;
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

    if (index == current_)
    {
        if (page.context)
        {
            page.context->invokeHide();
        }
        if (page.container && page.container->isAttached())
        {
            page.container->setVisible(false);
        }
        current_ = -1;
    }

    if (page.context)
    {
        page.context->invokeDestroy();
        page.context->clearWidgets();
    }

    lv_obj_t *obj = page.container && page.container->isAttached() ? page.container->getLvglItem() : nullptr;
    if (obj != nullptr && lv_obj_is_valid(obj))
    {
        lv_obj_delete(obj);
    }

    page.container.reset();
    page.context.reset();
    page.built = false;
}

void PageHost::release(int index)
{
    const bool was_current = (index == current_);
    destroyPage(index);
    if (was_current)
    {
        refreshFocus();
    }
}

void PageHost::releaseAll()
{
    for (int i = 0; i < static_cast<int>(pages_.size()); ++i)
    {
        destroyPage(i);
    }
    current_ = -1;
}

void PageHost::setCachePolicy(CachePolicy policy, int maxCached)
{
    policy_     = policy;
    max_cached_ = std::max(0, maxCached);
    applyCachePolicy();
}

void PageHost::applyCachePolicy()
{
    if (policy_ != CachePolicy::MaxCached)
    {
        return;
    }

    while (true)
    {
        int           victim      = -1;
        int           built_other = 0;
        std::uint32_t oldest      = std::numeric_limits<std::uint32_t>::max();

        for (int i = 0; i < static_cast<int>(pages_.size()); ++i)
        {
            Page &page = *pages_[i];
            if (!page.built || i == current_)
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
    // The host lv_obj was deleted externally; page objects are already gone.
    // Run page lifecycle and drop wrappers, but do not touch freed lv_objs.
    releaseAll();
}

} // namespace lvglpp::widgets
