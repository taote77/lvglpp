#ifndef LVGLPP_PAGEHOST_H
#define LVGLPP_PAGEHOST_H

#include "BaseItem.h"
#include "WidgetStore.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace lvglpp::widgets {

class PageHost;

/**
 * Per-page build context.
 *
 * Builders use emplace() to keep interactive wrappers alive for the lifetime
 * of the page, and can register lifecycle handlers through onShow/onHide/
 * onDestroy. The context and its widget store are destroyed when the page is
 * released.
 */
class PageContext
{
public:
    using LifecycleHandler = std::function<void()>;

    explicit PageContext(BaseItem *root) : root_(root)
    {}

    BaseItem *root() const
    {
        return root_;
    }

    WidgetStore &widgets()
    {
        return widgets_;
    }

    template <class T, class... Args>
    T *emplace(Args &&...args)
    {
        return widgets_.emplace<T>(std::forward<Args>(args)...);
    }

    PageContext &onShow(LifecycleHandler handler)
    {
        show_handler_ = std::move(handler);
        return *this;
    }

    PageContext &onHide(LifecycleHandler handler)
    {
        hide_handler_ = std::move(handler);
        return *this;
    }

    PageContext &onDestroy(LifecycleHandler handler)
    {
        destroy_handler_ = std::move(handler);
        return *this;
    }

private:
    friend class PageHost;

    void invokeShow();
    void invokeHide();
    void invokeDestroy();
    void clearWidgets();

    BaseItem        *root_ = nullptr;
    WidgetStore      widgets_;
    LifecycleHandler show_handler_;
    LifecycleHandler hide_handler_;
    LifecycleHandler destroy_handler_;
};

/**
 * Manages a set of full-size pages inside one container.
 *
 * Features:
 *  - lazy page construction on first show();
 *  - onShow/onHide/onDestroy page lifecycle;
 *  - KeepAll / ReleaseOnHide / MaxCached policies;
 *  - automatic keyboard focus refresh after visibility changes.
 *
 * UI-thread only.
 */
class PageHost : public BaseItem
{
public:
    using Builder = std::function<void(PageContext &)>;

    enum class CachePolicy {
        KeepAll,
        ReleaseOnHide,
        MaxCached,
    };

    explicit PageHost(BaseItem *parent = nullptr);

    ~PageHost() override;

    /// Add a page. Returns its index, or -1 for an invalid builder.
    int addPage(std::string name, Builder builder, bool lazy = true);

    int pageCount() const
    {
        return static_cast<int>(pages_.size());
    }

    int currentIndex() const
    {
        return current_;
    }

    std::string pageName(int index) const;

    /// Show a page, building it on first use. Returns true when switched.
    bool show(int index);

    /// Built page or nullptr.
    BaseItem *page(int index) const;

    /// Build a page without showing it. Returns nullptr for an invalid index.
    BaseItem *ensurePage(int index);

    BaseItem *currentPage() const;

    bool isBuilt(int index) const;

    /// Destroy a page (and its widgets). The current page becomes hidden.
    void release(int index);

    void releaseAll();

    void setCachePolicy(CachePolicy policy, int maxCached = 0);

    void setOnPageChanged(std::function<void(int, BaseItem *)> handler)
    {
        page_changed_ = std::move(handler);
    }

protected:
    void onLvglObjectDeleted() override;

private:
    struct Page;

    BaseItem *buildPage(int index);

    void destroyPage(int index);

    void applyCachePolicy();

    void refreshFocus();

    std::vector<std::unique_ptr<Page>> pages_;

    int current_ = -1;

    CachePolicy policy_ = CachePolicy::KeepAll;

    int max_cached_ = 0;

    std::function<void(int, BaseItem *)> page_changed_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_PAGEHOST_H
