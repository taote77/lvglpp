#ifndef LVGLPP_INPUTMANAGER_H
#define LVGLPP_INPUTMANAGER_H

#include "core/base/Connection.h"
#include "core/widgets/KeyEvent.h"
#include "lvgl.h"

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace lvglpp::widgets {
class Widget;
}

namespace lvglpp::sys {

/**
 * Owns keyboard focus scopes, the virtual keypad indev and global key routing.
 *
 * Threading: all methods except postKey()/postKeyRaw() must be called on the
 * UI thread. Posted keys are queued thread-safely and consumed by the virtual
 * keypad indev on the UI thread.
 */
/// Priority used by both focus scopes and global back handlers. Higher values
/// win; equal priorities are resolved LIFO.
enum class BackPriority : std::int32_t {
    App          = -100,
    Activity     = 0,
    Page         = 100,
    Popup        = 200,
    Dialog       = 300,
    SystemDialog = 400,
};

class InputManager
{
public:
    using ScopeToken       = std::uint64_t;
    using BackToken        = std::uint64_t;
    using GlobalKeyHandler = std::function<void(widgets::KeyEvent &)>;
    using BackHandler      = std::function<bool()>; // true = consumed

    static InputManager &getInstance();

    InputManager(const InputManager &)            = delete;
    InputManager &operator=(const InputManager &) = delete;

    /// Idempotent. Creates the virtual keypad indev if needed.
    void install();

    /// Release indevs/groups. Intended for application shutdown and tests.
    void shutdown();

    bool installed() const
    {
        return installed_;
    }

    /// Push a focus scope. It only becomes active when it is the highest
    /// priority visible scope.
    ScopeToken pushScope(widgets::Widget *root, BackHandler back = {}, BackPriority priority = BackPriority::Activity);

    /// Push a modal focus scope (dialog/popup).
    ScopeToken pushModalScope(widgets::Widget *root, BackHandler back = {}, BackPriority priority = BackPriority::Dialog);

    void popScope(ScopeToken token);

    /// Re-collect focusable widgets under the scope root.
    void refresh(ScopeToken token);

    /**
     * Refresh the top-most visible focus scope containing `item` (e.g. after
     * page visibility changes). Returns true when a scope was refreshed.
     */
    bool refreshScopeContaining(widgets::Widget *item);

    /// Add/remove one widget when its focusable flag changes.
    void refreshFor(widgets::Widget *item);

    bool requestFocus(widgets::Widget *item);

    widgets::Widget *focusedItem() const;

    void focusFirst();

    void focusLast();

    /// Thread-safe. Queued for the virtual keypad indev.
    void postKey(widgets::Key key, bool pressed);

    /// Thread-safe raw key variant (supports Character keys).
    void postKeyRaw(uint32_t raw, bool pressed);

    /// Dispatch on the UI thread. Returns true when consumed.
    bool dispatchKey(widgets::KeyEvent &event);

    bool dispatchGlobalKey(widgets::KeyEvent &event);

    core::Connection addGlobalKeyHandler(GlobalKeyHandler handler);

    /// Register a global back handler (used for App/Activity fallbacks).
    BackToken addBackHandler(BackPriority priority, BackHandler handler);

    void removeBackHandler(BackToken token);

    /// Run the unified back chain: scopes high-to-low, then global back
    /// handlers, then legacy fallback. Returns true when consumed.
    bool dispatchBack();

    lv_group_t *activeGroup() const;

    lv_indev_t *virtualIndev() const
    {
        return virtual_indev_;
    }

private:
    InputManager() = default;

    ~InputManager() = default;

    struct Scope {
        ScopeToken       id       = 0;
        widgets::Widget *root     = nullptr;
        lv_group_t      *group    = nullptr;
        BackPriority     priority = BackPriority::Activity;
        BackHandler      back;
        widgets::Widget *sink = nullptr;
    };

    struct BackEntry {
        BackToken    id       = 0;
        BackPriority priority = BackPriority::App;
        BackHandler  handler;
    };

    struct GlobalSlot {
        std::uint64_t                                  id = 0;
        std::shared_ptr<core::detail::ConnectionState> connection;
        GlobalKeyHandler                               callback;
    };

    struct PostedKey {
        uint32_t raw     = 0;
        bool     pressed = false;
    };

    static void virtualReadCb(lv_indev_t *indev, lv_indev_data_t *data);

    void fillKeyData(lv_indev_data_t *data);

    void syncIndevs();

    Scope *topVisibleScope();

    const Scope *topVisibleScope() const;

    void activateTopScope();

    void activateScope(Scope &scope);

    void deactivateScope(Scope &scope);

    void collectFocusables(lv_obj_t *obj, std::vector<lv_obj_t *> &out) const;

    bool isUnderScope(const Scope &scope, const widgets::Widget *item) const;

    void removeGlobalHandler(std::uint64_t id);

    ScopeToken next_scope_id_ = 1;

    std::vector<Scope> scopes_;

    lv_indev_t *virtual_indev_ = nullptr;

    bool installed_ = false;

    std::vector<GlobalSlot> global_handlers_;
    std::uint64_t           next_global_id_ = 0;

    std::vector<BackEntry> back_entries_;
    BackToken              next_back_token_ = 1;

    std::mutex            queue_mutex_;
    std::deque<PostedKey> queue_;

    // Virtual indev state (UI thread only).
    uint32_t active_raw_    = 0;
    uint32_t press_tick_    = 0;
    bool     long_sent_     = false;
    uint32_t long_press_ms_ = 500;
};

} // namespace lvglpp::sys

#endif // LVGLPP_INPUTMANAGER_H
