#if defined(LVGLPP_PLATFORM_SDL)

#include "SdlPlatform.h"

#include "core/log/log.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace lvglpp::sys {

namespace {

bool envFlag(const char *name)
{
    const char *value = std::getenv(name);
    return value != nullptr
        && (std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0
            || std::strcmp(value, "yes") == 0 || std::strcmp(value, "on") == 0);
}

int envInt(const char *name, int fallback)
{
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0')
    {
        return fallback;
    }
    char *end = nullptr;
    const long parsed = std::strtol(value, &end, 10);
    return (end != value) ? static_cast<int>(parsed) : fallback;
}

lv_display_rotation_t rotationFromDegrees(int degrees)
{
    int normalized = degrees % 360;
    if (normalized < 0)
    {
        normalized += 360;
    }
    switch (normalized)
    {
    case 90:
        return LV_DISPLAY_ROTATION_90;
    case 180:
        return LV_DISPLAY_ROTATION_180;
    case 270:
        return LV_DISPLAY_ROTATION_270;
    default:
        return LV_DISPLAY_ROTATION_0;
    }
}

} // namespace

SdlPlatform::SdlPlatform(int argc, char **argv)
{
    (void)argc;
    (void)argv;
}

SdlPlatform::~SdlPlatform()
{
    shutdown();
}

bool SdlPlatform::init()
{
    // Must be set before any SDL initialization so SDL does not install its
    // own SIGINT/SIGTERM handlers; Application owns signal handling.
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0)
    {
        LOG_ERROR() << "SDL event subsystem init failed: " << SDL_GetError();
        return false;
    }
    events_inited_ = true;

    // SDL2 exposes one global event filter (SDL_AddEventFilter is SDL3).
    // Save the previous one so we do not break a host application.
    SDL_GetEventFilter(&previous_filter_, &previous_filter_data_);
    SDL_SetEventFilter(&SdlPlatform::eventFilter, this);
    filter_installed_ = true;

    display_ = lv_sdl_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    if (display_ == nullptr)
    {
        LOG_ERROR() << "failed to create the SDL window/display";
        shutdown();
        return false;
    }
    // From here on lv_deinit() owns SDL teardown via release_disp_cb.
    driver_owns_sdl_ = true;
    lv_display_set_default(display_);

    native_width_    = lv_display_get_horizontal_resolution(display_);
    native_height_   = lv_display_get_vertical_resolution(display_);
    fixed_panel_     = envFlag("LVGLPP_SDL_FIXED_PANEL");
    rotation_offset_ = envInt("LVGLPP_PANEL_ROTATION_OFFSET", 0);

#if LV_DRAW_TRANSFORM_USE_MATRIX
    // Required for visible rotation in DIRECT render mode.
    lv_display_set_matrix_rotation(display_, true);
#endif

    mouse_    = lv_sdl_mouse_create();
    keyboard_ = lv_sdl_keyboard_create();
    wheel_    = lv_sdl_mousewheel_create();
    sim_      = lv_sdl_mouse_create();

    if (mouse_ == nullptr || keyboard_ == nullptr || sim_ == nullptr)
    {
        LOG_ERROR() << "failed to create SDL input devices";
        shutdown();
        return false;
    }

    lv_indev_set_display(mouse_, display_);
    lv_indev_set_display(keyboard_, display_);
    if (wheel_ != nullptr)
    {
        lv_indev_set_display(wheel_, display_);
    }

    lv_indev_set_display(sim_, display_);
    lv_indev_set_type(sim_, LV_INDEV_TYPE_POINTER);
    lv_indev_set_user_data(sim_, this);
    lv_indev_set_read_cb(sim_, &SdlPlatform::simPointerRead);

    return true;
}

void SdlPlatform::shutdown()
{
    if (filter_installed_)
    {
        SDL_SetEventFilter(previous_filter_, previous_filter_data_);
        filter_installed_     = false;
        previous_filter_      = nullptr;
        previous_filter_data_ = nullptr;
    }

    if (events_inited_ && !driver_owns_sdl_)
    {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
    events_inited_   = false;
    driver_owns_sdl_ = false;

    display_  = nullptr;
    mouse_    = nullptr;
    keyboard_ = nullptr;
    wheel_    = nullptr;
    sim_      = nullptr;
}

int SdlPlatform::eventFilter(void *userdata, SDL_Event *event)
{
    auto *self = static_cast<SdlPlatform *>(userdata);
    if (self == nullptr || event == nullptr)
    {
        return 1;
    }

    if (event->type == SDL_QUIT || (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_CLOSE))
    {
        // Drop the event so LVGL's SDL driver never runs its lv_deinit/exit(0)
        // path; Application performs an ordered shutdown instead.
        self->notifyQuit();
        return 0;
    }

    if (self->previous_filter_ != nullptr)
    {
        return self->previous_filter_(self->previous_filter_data_, event);
    }
    return 1;
}

void SdlPlatform::simPointerRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    auto *self = static_cast<SdlPlatform *>(lv_indev_get_user_data(indev));
    if (self == nullptr)
    {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    data->point = self->sim_position_;
    data->state = self->sim_pressed_ ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void SdlPlatform::simulatePointerClick(std::int32_t x, std::int32_t y)
{
    if (sim_ == nullptr || display_ == nullptr)
    {
        return;
    }

    // Automation passes logical UI coordinates; the LVGL core rotates raw
    // indev coordinates forward, so undo the display rotation here.
    const lv_display_rotation_t rotation = lv_display_get_rotation(display_);
    const std::int32_t nativeWidth       = lv_display_get_original_horizontal_resolution(display_);
    const std::int32_t nativeHeight      = lv_display_get_original_vertical_resolution(display_);

    std::int32_t rawX = x;
    std::int32_t rawY = y;
    switch (rotation)
    {
    case LV_DISPLAY_ROTATION_90:
        rawX = y;
        rawY = nativeHeight - x - 1;
        break;
    case LV_DISPLAY_ROTATION_180:
        rawX = nativeWidth - x - 1;
        rawY = nativeHeight - y - 1;
        break;
    case LV_DISPLAY_ROTATION_270:
        rawX = nativeWidth - y - 1;
        rawY = x;
        break;
    case LV_DISPLAY_ROTATION_0:
    default:
        break;
    }

    sim_position_.x = static_cast<lv_coord_t>(rawX);
    sim_position_.y = static_cast<lv_coord_t>(rawY);

    sim_pressed_ = true;
    lv_indev_read(sim_);
    lv_refr_now(display_);
    lv_timer_handler();

    sim_pressed_ = false;
    lv_indev_read(sim_);
    lv_refr_now(display_);
}

bool SdlPlatform::applyOrientation(Orientation orientation)
{
    if (display_ == nullptr)
    {
        return false;
    }

    const int degrees = static_cast<int>(orientation) + rotation_offset_;

    if (fixed_panel_)
    {
        // LVGL 9.3's DIRECT + matrix transform path does not reliably support
        // 90/270 on the SDL texture backend (memory corruption at 90°). Use the
        // fixed-panel path only for 0/180; fall back to window reshape for the
        // portrait directions. FBDEV is the reference rotation backend.
        if (orientation == Orientation::Portrait || orientation == Orientation::ReversePortrait)
        {
            LOG_WARN() << "SDL fixed-panel 90/270 is unsupported; using window reshape instead";
        }
        else
        {
            lv_display_set_resolution(display_, native_width_, native_height_);
            lv_display_set_rotation(display_, rotationFromDegrees(degrees));
            return true;
        }
    }

    // Desktop mode: reshape the window to the logical orientation. 90/270
    // become a portrait window; 180/270 additionally rotate the content so
    // the four gyro directions remain distinguishable.
    const bool  portrait   = orientation == Orientation::Portrait || orientation == Orientation::ReversePortrait;
    const bool  reverse    = orientation == Orientation::ReverseLandscape || orientation == Orientation::ReversePortrait;
    const int   shortSide  = std::min(native_width_, native_height_);
    const int   longSide   = std::max(native_width_, native_height_);

    lv_display_set_resolution(display_, portrait ? shortSide : longSide, portrait ? longSide : shortSide);
    lv_display_set_rotation(display_, reverse ? LV_DISPLAY_ROTATION_180 : LV_DISPLAY_ROTATION_0);
    return true;
}

void SdlPlatform::setTheme(lv_theme_t *theme)
{
    if (theme == nullptr)
    {
        LOG_WARN() << "theme invalid";
        return;
    }
    if (display_ != nullptr)
    {
        lv_display_set_theme(display_, theme);
    }
}

} // namespace lvglpp::sys

#endif // LVGLPP_PLATFORM_SDL
