#ifndef LVGLPP_PAGEHOST_H
#define LVGLPP_PAGEHOST_H

#include "Widget.h"
#include "WidgetStore.h"
#include "core/sys/InputManager.h"

#include <any>
#include <cstdint>
#include <deque>
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
 * of the page, and can register lifecycle handlers through
 * onArguments/onShow/onHide/onPause/onResume/onDestroy/onResult. The context
 * and its widget store are destroyed when the page is released.
 */
class PageContext
{
public:
    using LifecycleHandler = std::function<void()>;
    using ArgumentsHandler = std::function<void(const std::any &)>;
    using ResultHandler    = std::function<void(std::any)>;

    explicit PageContext(Widget *root) : root_(root)
    {}

    Widget *root() const
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

    /// Arguments supplied by show()/push() for this page instance.
    const std::any &arguments() const
    {
        return arguments_;
    }

    /// Called once after the first build with the entry arguments.
    PageContext &onArguments(ArgumentsHandler handler)
    {
        arguments_handler_ = std::move(handler);
        return *this;
    }

    /// Called when an already-built page is pushed again with new arguments.
    PageContext &onNewArguments(ArgumentsHandler handler)
    {
        new_arguments_handler_ = std::move(handler);
        return *this;
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

    PageContext &onResume(LifecycleHandler handler)
    {
        resume_handler_ = std::move(handler);
        return *this;
    }

    PageContext &onPause(LifecycleHandler handler)
    {
        pause_handler_ = std::move(handler);
        return *this;
    }

    PageContext &onDestroy(LifecycleHandler handler)
    {
        destroy_handler_ = std::move(handler);
        return *this;
    }

    /// Result from the page above when it pops back to this page.
    PageContext &onResult(ResultHandler handler)
    {
        result_handler_ = std::move(handler);
        return *this;
    }

private:
    friend class PageHost;

    void setArguments(std::any args);
    void invokeArguments();
    void invokeNewArguments();
    void invokeShow();
    void invokeHide();
    void invokeResume();
    void invokePause();
    void invokeDestroy();
    void invokeResult(std::any result);
    void clearWidgets();

    Widget          *root_ = nullptr;
    WidgetStore      widgets_;
    std::any         arguments_;
    ArgumentsHandler arguments_handler_;
    ArgumentsHandler new_arguments_handler_;
    LifecycleHandler show_handler_;
    LifecycleHandler hide_handler_;
    LifecycleHandler resume_handler_;
    LifecycleHandler pause_handler_;
    LifecycleHandler destroy_handler_;
    ResultHandler    result_handler_;
};

/**
 * Manages a set of lazy pages inside one container.
 *
 * Root pages behave like bottom-navigation tabs; each root owns its own back
 * stack. Non-root pages are pushed on top of the active root. The host
 * registers a Page-priority back handler that pops the active stack before
 * the containing Activity handles back.
 *
 * UI-thread only.
 */
class PageHost : public Widget
{
public:
    using Builder = std::function<void(PageContext &)>;

    enum class CachePolicy {
        KeepAll,
        ReleaseOnHide,
        MaxCached,
    };

    struct StateEntry {
        std::string name;
        std::any    args;
    };

    struct State {
        std::vector<std::vector<StateEntry>> stacks;
        std::string                          activeRoot;
    };

    explicit PageHost(Widget *parent = nullptr);

    ~PageHost() override;

    /// Add a page. Root pages are tab destinations; non-root pages can be
    /// pushed onto the active root's stack. Returns -1 for an invalid builder.
    int addPage(std::string name, Builder builder, bool lazy = true, bool root = true);

    int pageCount() const
    {
        return static_cast<int>(pages_.size());
    }

    int currentIndex() const
    {
        return current_;
    }

    int currentRoot() const
    {
        return active_root_;
    }

    std::string pageName(int index) const;

    int indexOf(const std::string &name) const;

    /// Select a root/tab. Reselecting the active root pops its stack to root.
    bool show(int index);

    bool show(const std::string &name);

    /// Push a non-root page onto the active root's stack.
    bool push(int index, std::any args = {});

    bool push(const std::string &name, std::any args = {});

    /// Pop the active root's top page. Returns false at root depth.
    bool pop(std::any result = {});

    bool popTo(int index, std::any result = {});

    bool popToRoot();

    /// Back-handler convenience: pop() when possible.
    bool back();

    int depth() const;

    std::vector<int> history() const;

    /// Built page or nullptr.
    Widget *page(int index) const;

    /// Build a page without showing it. Returns nullptr for an invalid index.
    Widget *ensurePage(int index);

    Widget *currentPage() const;

    bool isBuilt(int index) const;

    /// Destroy a page (and its widgets). The page's stack entries are kept.
    void release(int index);

    void releaseAll();

    void setCachePolicy(CachePolicy policy, int maxCached = 0);

    void setOnPageChanged(std::function<void(int, Widget *)> handler)
    {
        page_changed_ = std::move(handler);
    }

    State saveState() const;

    /// Restore stacks/arguments after rebuilding the host. Routes must have
    /// been added with the same names.
    bool restoreState(const State &state);

protected:
    void onLvglObjectDeleted() override;

private:
    struct Entry {
        int      page = -1;
        std::any args;
    };

    struct Page;

    Widget *buildPage(int index, const std::any &args);

    void destroyPage(int index);

    void releaseAllNow();

    void applyCachePolicy();

    void refreshFocus();

    bool doShow(int index);

    bool doPush(int index, std::any args);

    bool doPop(std::any result);

    bool doPopTo(int index, std::any result);

    void activateEntry(const Entry &entry, const std::any *result, bool new_push);

    void deactivateCurrent(bool destroy);

    bool inActiveStack(int pageIndex) const;

    void processPendingOps();

    void queueOrRun(std::function<void()> op);

    std::vector<std::unique_ptr<Page>> pages_;

    /// One stack per root page, indexed by root page index.
    std::vector<std::vector<Entry>> stacks_;

    int  active_root_ = -1;
    int  current_     = -1;
    bool navigating_  = false;

    CachePolicy policy_ = CachePolicy::KeepAll;

    int max_cached_ = 0;

    std::function<void(int, Widget *)> page_changed_;

    std::deque<std::function<void()>> pending_ops_;

    std::uint64_t back_token_ = 0;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_PAGEHOST_H
