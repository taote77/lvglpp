#ifndef LVGLPP_SDLPLATFORM_H
#define LVGLPP_SDLPLATFORM_H

#if defined(LVGLPP_PLATFORM_SDL)

#include "core/sys/platform/Platform.h"

#include <cstdint>

#include <SDL2/SDL.h>
#include <lvgl.h>

namespace lvglpp::sys {

class SdlPlatform final : public Platform
{
public:
    SdlPlatform(int argc, char **argv);
    ~SdlPlatform() override;

    const char *name() const noexcept override
    {
        return "SDL";
    }

    bool init() override;
    void shutdown() override;

    lv_display_t *display() const override
    {
        return display_;
    }

    void setTheme(lv_theme_t *theme) override;
    void simulatePointerClick(std::int32_t x, std::int32_t y) override;

    bool supportsPanelRotation() const noexcept override
    {
        return true;
    }

    bool supportsWindowReshape() const noexcept override
    {
        return true;
    }

    bool applyOrientation(Orientation orientation) override;

    bool providesTick() const noexcept override
    {
        return true;
    }

private:
    static int  eventFilter(void *userdata, SDL_Event *event);
    static void simPointerRead(lv_indev_t *indev, lv_indev_data_t *data);

    lv_display_t *display_  = nullptr;
    lv_indev_t   *mouse_    = nullptr;
    lv_indev_t   *keyboard_ = nullptr;
    lv_indev_t   *wheel_    = nullptr;
    lv_indev_t   *sim_      = nullptr;

    lv_point_t sim_position_{};
    bool       sim_pressed_ = false;

    bool            events_inited_        = false;
    bool            filter_installed_     = false;
    bool            driver_owns_sdl_      = false;
    SDL_EventFilter previous_filter_      = nullptr;
    void           *previous_filter_data_ = nullptr;

    std::int32_t native_width_     = 0;
    std::int32_t native_height_    = 0;
    bool         fixed_panel_      = false;
    int          rotation_offset_  = 0;
};

} // namespace lvglpp::sys

#endif // LVGLPP_PLATFORM_SDL

#endif // LVGLPP_SDLPLATFORM_H
