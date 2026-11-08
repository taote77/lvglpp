#ifndef LVGLPP_TOAST_H
#define LVGLPP_TOAST_H

#include "OverlayManager.h"
#include "ToastType.h"

#include <cstdint>
#include <memory>
#include <string>

namespace lvglpp::widgets {

class ToastView;

/**
 * Lightweight transient notification.
 *
 * UI thread only. A new message replaces the toast currently on screen and
 * restarts its display duration; messages are not queued, so rapid actions
 * never build up a backlog of stale confirmations.
 *
 * One ToastView is reused for the whole process. Its semantic colours are
 * refreshed on every show, so it follows Theme::setMode() even though the
 * overlay is not part of the rebuilt page tree. Application::shutdown()
 * tears it down explicitly.
 */
class Toast
{
public:
    static constexpr std::uint32_t kDefaultDurationMs = 3000;

    /// Show (or replace with) a message. durationMs == 0 uses the default.
    static void show(const std::string &message, ToastType type,
                     std::uint32_t durationMs = kDefaultDurationMs);

    static void success(const std::string &message);
    static void warn(const std::string &message);
    static void error(const std::string &message);

    /// Hide the current toast and cancel its pending timer.
    static void cancel();

    /// Destroy the process-lifetime view. Safe to call more than once.
    static void shutdown();

private:
    Toast();
    ~Toast();

    Toast(const Toast &)            = delete;
    Toast &operator=(const Toast &) = delete;

    static Toast &getInstance();
    static Toast *instanceIfCreated();
    static Toast *&instanceSlot();

    void showImpl(const std::string &message, ToastType type, std::uint32_t durationMs);
    void startShowAnimation();
    void scheduleHide(std::uint32_t durationMs);
    void startHideAnimation();
    void hideNow();
    void stopAnimations();

    enum class State { Idle, Showing, Visible, Hiding };

    std::unique_ptr<ToastView> view_;
    OverlayManager::Token      overlay_token_{};
    lv_anim_t                  show_anim_{};
    lv_anim_t                  hide_anim_{};
    lv_timer_t                *hide_timer_          = nullptr;
    std::uint32_t              current_duration_ms_ = kDefaultDurationMs;
    State                      state_               = State::Idle;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TOAST_H
