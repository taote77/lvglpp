#ifndef LVGLPP_UICONFIG_H
#define LVGLPP_UICONFIG_H

#include "OrientationFilter.h"
#include "UiTypes.h"
#include "core/base/Object.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>

#include <lvgl.h>

namespace lvglpp::sys {

class Platform;

/**
 * Runtime display/configuration state.
 *
 * All request* methods are thread-safe and only touch atomics.  The UI thread
 * consumes them in processPending(), which Application::exec() calls before
 * lv_timer_handler().  This keeps sensor threads and LVGL event callbacks from
 * rebuilding the widget tree synchronously.
 */
class UiConfig : public core::Object
{
public:
    static UiConfig &instance();

    UiConfig(const UiConfig &)            = delete;
    UiConfig &operator=(const UiConfig &) = delete;

    /// UI thread; call after the platform has created the display.
    void initialize(Platform *platform);

    /// UI thread; clears state before platform shutdown/lv_deinit.
    void shutdown();

    bool initialized() const noexcept
    {
        return initialized_;
    }

    const UiConfiguration &current() const noexcept
    {
        return current_;
    }

    std::int32_t width() const noexcept;
    std::int32_t height() const noexcept;
    bool isPortrait() const noexcept;
    bool isRtl() const noexcept;
    lv_display_rotation_t appliedRotation() const noexcept;

    // Thread-safe, non-blocking, coalescing requests.
    void requestOrientation(Orientation orientation) noexcept;
    void requestLayoutDirection(LayoutDirection direction) noexcept;
    void requestRebuild() noexcept;
    void feedGravity(float gx, float gy, float gz) noexcept;

    /// UI thread. Returns true when a configuration change was applied.
    bool processPending();

    /// UI thread; applies a configuration and optionally notifies listeners.
    void applyNow(const UiConfiguration &configuration, bool notify);

private:
    UiConfig() = default;

    void applyBaseDirection();
    OrientationFilter &orientationFilter();

    Platform                 *platform_ = nullptr;
    UiConfiguration           current_{};
    std::atomic<int>          pending_orientation_{-1};
    std::atomic<int>          pending_direction_{-1};
    std::atomic<bool>         pending_rebuild_{false};
    std::mutex                gravity_mutex_;
    std::unique_ptr<OrientationFilter> orientation_filter_;
    bool                      initialized_ = false;
};

} // namespace lvglpp::sys

#endif // LVGLPP_UICONFIG_H
