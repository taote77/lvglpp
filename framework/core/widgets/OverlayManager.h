#ifndef LVGLPP_OVERLAYMANAGER_H
#define LVGLPP_OVERLAYMANAGER_H

#include "Widget.h"
#include "core/sys/InputManager.h"
#include <cstdint>
#include <vector>

namespace lvglpp::widgets {

/**
 * Android WindowManager-inspired overlay stacking.
 *
 * Overlays are ordered by priority first and insertion/bring-to-front order
 * second. Focusable overlays own an InputManager modal scope; transients such
 * as Toast never take focus or consume back.
 */
class OverlayManager
{
public:
    enum class Priority : std::int32_t {
        Toast        = 10,
        Popup        = 200,
        Dialog       = 300,
        SystemDialog = 400,
    };

    struct Token {
        std::uint64_t id = 0;

        explicit operator bool() const noexcept
        {
            return id != 0;
        }
    };

    static OverlayManager &getInstance();

    OverlayManager(const OverlayManager &)            = delete;
    OverlayManager &operator=(const OverlayManager &) = delete;
    OverlayManager(OverlayManager &&)                 = delete;
    OverlayManager &operator=(OverlayManager &&)      = delete;

    /// Register a focusable overlay. The manager pushes a modal scope whose
    /// back handler is `onBack`. The root should already be visible.
    Token attach(Widget *root, Priority priority, sys::InputManager::BackHandler onBack);

    /// Unregister an overlay. Idempotent; safe from the overlay's own back
    /// handler or destructor.
    void detach(Token token);

    /// Raise within the same priority (higher priority overlays stay above).
    void bringToFront(Token token);

    bool isTopmost(Token token) const;

    Priority topPriority() const;

    int count(Priority priority) const;

    /// Non-focusable, non-blocking overlay (Toast). Never enters the back chain.
    Token registerTransient(Widget *root, Priority priority = Priority::Toast);
    void  unregisterTransient(Token token);

    /// Intended for tests/application shutdown.
    void shutdown();

private:
    struct Entry {
        std::uint64_t                 id        = 0;
        Priority                      priority  = Priority::Toast;
        std::uint64_t                 seq       = 0;
        Widget                       *root      = nullptr;
        sys::InputManager::ScopeToken scope     = 0;
        bool                          transient = false;
    };

    OverlayManager()  = default;
    ~OverlayManager() = default;

    Entry       *find(Token token);
    const Entry *find(Token token) const;
    const Entry *topEntry() const;
    void         restack();

    std::vector<Entry> entries_;
    std::uint64_t      next_id_    = 1;
    std::uint64_t      next_seq_   = 1;
    bool               restacking_ = false;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_OVERLAYMANAGER_H
