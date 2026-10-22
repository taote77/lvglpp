#ifndef LVGLPP_PLATFORM_H
#define LVGLPP_PLATFORM_H

#include "core/sys/UiTypes.h"

#include <cstdint>
#include <functional>
#include <utility>

#include <lvgl.h>

namespace lvglpp::sys {

enum class PlatformKind {
    Auto,
    Sdl,
    Framebuf,
    Drm,
};

/**
 * Display/input backend abstraction.
 *
 * A Platform owns only platform resources. LVGL owns the display and input
 * devices: Platform::shutdown() runs before lv_deinit() and must not delete
 * them (or call SDL_Quit); lv_deinit() releases the drivers.
 *
 * Application composes a Platform; backend types are not coupled to the
 * application lifecycle.
 */
class Platform
{
public:
    Platform()          = default;
    virtual ~Platform() = default;

    Platform(const Platform &)            = delete;
    Platform &operator=(const Platform &) = delete;

    virtual const char *name() const noexcept = 0;

    /// Called after lv_init() and asset/log setup. Creates display + indevs.
    virtual bool init() = 0;

    /// Called before lv_deinit(). Idempotent; must not delete LVGL objects.
    virtual void shutdown() = 0;

    virtual lv_display_t *display() const = 0;

    /// Null-safe; no-op when the backend has no theme support.
    virtual void setTheme(lv_theme_t *theme) = 0;

    /// UI-thread pointer simulation; no-op when unsupported.
    /// Coordinates are logical (already rotated) UI coordinates.
    virtual void simulatePointerClick(std::int32_t x, std::int32_t y) = 0;

    /// True when the backend can rotate a fixed-scanout panel in software.
    virtual bool supportsPanelRotation() const noexcept
    {
        return false;
    }

    /// True when the backend can reshape its output (SDL window, DRM mode).
    virtual bool supportsWindowReshape() const noexcept
    {
        return false;
    }

    /// UI-thread; applies one of the four logical orientations.
    virtual bool applyOrientation(Orientation orientation)
    {
        (void)orientation;
        return false;
    }

    /// True when the backend's LVGL driver already provides a tick source.
    virtual bool providesTick() const noexcept
    {
        return false;
    }

    using QuitHandler = std::function<void()>;

    /**
     * Registered by Application. Backends call notifyQuit() from event or
     * signal-adjacent callbacks; implementations must be non-blocking and
     * thread-safe (the handler only performs atomic stores).
     */
    void setQuitHandler(QuitHandler handler)
    {
        quit_handler_ = std::move(handler);
    }

protected:
    void notifyQuit() const
    {
        if (quit_handler_)
        {
            quit_handler_();
        }
    }

private:
    QuitHandler quit_handler_;
};

} // namespace lvglpp::sys

#endif // LVGLPP_PLATFORM_H
