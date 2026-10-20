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
class BaseItem;
}

namespace lvglpp::sys {

/**
 * Owns keyboard focus scopes, the virtual keypad indev and global key routing.
 *
 * Threading: all methods except postKey()/postKeyRaw() must be called on the
 * UI thread. Posted keys are queued thread-safely and consumed by the virtual
 * keypad indev on the UI thread.
 */
class InputManager
{
public:
    using ScopeToken       = std::uint64_t;
    using GlobalKeyHandler = std::function<void(widgets::KeyEvent &)>;
    using BackHandler      = std::function<void()>;

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

    /// Activity-level focus scope.
    ScopeToken pushScope(widgets::BaseItem *root, BackHandler back = {});

    /// Modal (dialog) focus scope.
    ScopeToken pushModalScope(widgets::BaseItem *root, BackHandler back = {});

    void popScope(ScopeToken token);

    /// Re-collect focusable widgets under the scope root.
    void refresh(ScopeToken token);

    /**
     * Refresh the top-most focus scope containing `item` (e.g. after page
     * visibility changes). Does nothing when a modal scope is on top.
     * Returns true when a scope was refreshed.
     */
    bool refreshScopeContaining(widgets::BaseItem *item);

    /// Add/remove one widget when its focusable flag changes.
    void refreshFor(widgets::BaseItem *item);

    bool requestFocus(widgets::BaseItem *item);

    widgets::BaseItem *focusedItem() const;

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

    /// Fallback for an unconsumed Escape when the active scope has no handler.
    void setBackHandler(BackHandler handler);

    lv_group_t *activeGroup() const;

    lv_indev_t *virtualIndev() const
    {
        return virtual_indev_;
    }

private:
    InputManager() = default;

    ~InputManager() = default;

    struct Scope {
        ScopeToken         id    = 0;
        widgets::BaseItem *root  = nullptr;
        lv_group_t        *group = nullptr;
        bool               modal = false;
        BackHandler        back;
        widgets::BaseItem *sink = nullptr;
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

    void activateScope(Scope &scope);

    void deactivateScope(Scope &scope);

    void collectFocusables(lv_obj_t *obj, std::vector<lv_obj_t *> &out) const;

    bool isUnderScope(const Scope &scope, const widgets::BaseItem *item) const;

    void removeGlobalHandler(std::uint64_t id);

    ScopeToken next_scope_id_ = 1;

    std::vector<Scope> scopes_;

    lv_indev_t *virtual_indev_ = nullptr;

    bool installed_ = false;

    std::vector<GlobalSlot> global_handlers_;
    std::uint64_t           next_global_id_ = 0;

    BackHandler back_handler_;

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
