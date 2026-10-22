#include "UiConfig.h"

#include "StackView.h"
#include "core/sys/platform/Platform.h"
#include "core/log/log.h"

#include <cstdlib>
#include <cstring>
#include <string>

namespace lvglpp::sys {

namespace {

const char *envValue(const char *name)
{
    const char *value = std::getenv(name);
    return (value != nullptr && *value != '\0') ? value : nullptr;
}

bool envFlag(const char *name)
{
    const char *value = envValue(name);
    if (value == nullptr)
    {
        return false;
    }
    return std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0
        || std::strcmp(value, "yes") == 0 || std::strcmp(value, "on") == 0;
}

Orientation parseOrientation(const char *value, Orientation fallback)
{
    if (value == nullptr)
    {
        return fallback;
    }
    if (std::strcmp(value, "landscape") == 0 || std::strcmp(value, "0") == 0)
    {
        return Orientation::Landscape;
    }
    if (std::strcmp(value, "portrait") == 0 || std::strcmp(value, "90") == 0)
    {
        return Orientation::Portrait;
    }
    if (std::strcmp(value, "reverse-landscape") == 0 || std::strcmp(value, "180") == 0)
    {
        return Orientation::ReverseLandscape;
    }
    if (std::strcmp(value, "reverse-portrait") == 0 || std::strcmp(value, "270") == 0)
    {
        return Orientation::ReversePortrait;
    }
    LOG_WARN() << "unknown LVGLPP_ORIENTATION value: " << value;
    return fallback;
}

std::int32_t displayWidth()
{
    lv_display_t *display = lv_display_get_default();
    return display != nullptr ? lv_display_get_horizontal_resolution(display) : 800;
}

std::int32_t displayHeight()
{
    lv_display_t *display = lv_display_get_default();
    return display != nullptr ? lv_display_get_vertical_resolution(display) : 480;
}

} // namespace

UiConfig &UiConfig::instance()
{
    static UiConfig config;
    return config;
}

void UiConfig::initialize(Platform *platform)
{
    platform_ = platform;

#if LVGLPP_ENABLE_ROTATION
    const Orientation initialOrientation =
        parseOrientation(envValue("LVGLPP_ORIENTATION"), Orientation::Landscape);

    if (platform_ != nullptr)
    {
        if (!platform_->applyOrientation(initialOrientation) && initialOrientation != Orientation::Landscape)
        {
            LOG_WARN() << "platform cannot apply orientation "
                       << static_cast<int>(initialOrientation) << "°, keeping native orientation";
        }
    }
    current_.orientation = initialOrientation;
#endif

#if LVGLPP_ENABLE_RTL
    current_.direction = envFlag("LVGLPP_RTL") ? LayoutDirection::Rtl : LayoutDirection::Ltr;
#endif

    current_.width  = displayWidth();
    current_.height = displayHeight();
    ++current_.revision;

    applyBaseDirection();
#if LVGLPP_ENABLE_ROTATION
    orientationFilter().setCurrentOrientation(current_.orientation);
#endif
    initialized_ = true;
}

void UiConfig::shutdown()
{
    initialized_ = false;
    platform_    = nullptr;
    pending_orientation_.store(-1, std::memory_order_release);
    pending_direction_.store(-1, std::memory_order_release);
    pending_rebuild_.store(false, std::memory_order_release);
    {
        std::lock_guard<std::mutex> lock(gravity_mutex_);
        orientation_filter_.reset();
    }
}

std::int32_t UiConfig::width() const noexcept
{
    return initialized_ ? current_.width : displayWidth();
}

std::int32_t UiConfig::height() const noexcept
{
    return initialized_ ? current_.height : displayHeight();
}

bool UiConfig::isPortrait() const noexcept
{
    return initialized_ ? current_.isPortrait() : (displayHeight() > displayWidth());
}

bool UiConfig::isRtl() const noexcept
{
#if LVGLPP_ENABLE_RTL
    return initialized_ ? current_.isRtl() : false;
#else
    return false;
#endif
}

lv_display_rotation_t UiConfig::appliedRotation() const noexcept
{
    lv_display_t *display = lv_display_get_default();
    return display != nullptr ? lv_display_get_rotation(display) : LV_DISPLAY_ROTATION_0;
}

void UiConfig::requestOrientation(Orientation orientation) noexcept
{
#if LVGLPP_ENABLE_ROTATION
    pending_orientation_.store(static_cast<int>(orientation), std::memory_order_release);
#else
    (void)orientation;
#endif
}

void UiConfig::requestLayoutDirection(LayoutDirection direction) noexcept
{
#if LVGLPP_ENABLE_RTL
    pending_direction_.store(static_cast<int>(direction), std::memory_order_release);
#else
    (void)direction;
#endif
}

void UiConfig::requestRebuild() noexcept
{
    pending_rebuild_.store(true, std::memory_order_release);
}

OrientationFilter &UiConfig::orientationFilter()
{
    std::lock_guard<std::mutex> lock(gravity_mutex_);
    if (!orientation_filter_)
    {
        orientation_filter_ = std::make_unique<OrientationFilter>();
    }
    return *orientation_filter_;
}

void UiConfig::feedGravity(float gx, float gy, float gz) noexcept
{
#if LVGLPP_ENABLE_ROTATION
    std::optional<Orientation> next;
    {
        std::lock_guard<std::mutex> lock(gravity_mutex_);
        if (!orientation_filter_)
        {
            orientation_filter_ = std::make_unique<OrientationFilter>();
            orientation_filter_->setCurrentOrientation(current_.orientation);
        }
        next = orientation_filter_->update(gx, gy, gz);
    }
    if (next)
    {
        requestOrientation(*next);
    }
#else
    (void)gx;
    (void)gy;
    (void)gz;
#endif
}

void UiConfig::applyBaseDirection()
{
#if !LVGLPP_ENABLE_RTL
    return;
#else
    const lv_base_dir_t base = current_.isRtl() ? LV_BASE_DIR_RTL : LV_BASE_DIR_LTR;

    if (lv_obj_t *screen = lv_screen_active())
    {
        lv_obj_set_style_base_dir(screen, base, 0);
    }
    if (lv_obj_t *layer = lv_layer_top())
    {
        lv_obj_set_style_base_dir(layer, base, 0);
    }
    if (lv_obj_t *layer = lv_layer_bottom())
    {
        lv_obj_set_style_base_dir(layer, base, 0);
    }
    if (lv_obj_t *layer = lv_layer_sys())
    {
        lv_obj_set_style_base_dir(layer, base, 0);
    }
#endif
}

bool UiConfig::processPending()
{
    if (!initialized_ || platform_ == nullptr)
    {
        return false;
    }

    // Never delete the widget tree while a StackView transition is running.
    if (StackView::getInstance().isAnimating())
    {
        return false;
    }

    bool            changed            = false;
    bool            orientationApplied = false;
    Orientation     nextOrientation    = current_.orientation;
    LayoutDirection nextDirection      = current_.direction;

#if LVGLPP_ENABLE_ROTATION
    const int requestedOrientation = pending_orientation_.exchange(-1, std::memory_order_acq_rel);
    if (requestedOrientation >= 0)
    {
        const Orientation orientation = normalizeOrientation(requestedOrientation);
        if (orientation != current_.orientation)
        {
            if (platform_->applyOrientation(orientation))
            {
                nextOrientation    = orientation;
                orientationApplied = true;
                changed            = true;
            }
            else
            {
                LOG_WARN() << "failed to apply orientation " << static_cast<int>(orientation) << "°";
            }
        }
    }
#endif

#if LVGLPP_ENABLE_RTL
    const int requestedDirection = pending_direction_.exchange(-1, std::memory_order_acq_rel);
    if (requestedDirection >= 0)
    {
        const LayoutDirection direction =
            requestedDirection == static_cast<int>(LayoutDirection::Rtl) ? LayoutDirection::Rtl : LayoutDirection::Ltr;
        if (direction != current_.direction)
        {
            nextDirection = direction;
            changed       = true;
        }
    }
#endif

    const bool requestedRebuild = pending_rebuild_.exchange(false, std::memory_order_acq_rel);

    if (!changed && !requestedRebuild)
    {
        return false;
    }

    current_.orientation = nextOrientation;
    current_.direction   = nextDirection;
    if (orientationApplied)
    {
        // A direction-only or language-rebuild transition must not re-read the
        // display: window managers may report a slightly different window size
        // (e.g. 800x487) and that would silently change the layout contract.
        current_.width  = displayWidth();
        current_.height = displayHeight();
    }
    ++current_.revision;

    applyBaseDirection();

    // Dialogs and other transient listeners close before activities rebuild.
    emitSignal("configurationChanged");

    StackView::getInstance().cancelRootAnimations();
    StackView::getInstance().notifyConfigurationChanged(current_);

    if (lv_obj_t *screen = lv_screen_active())
    {
        lv_obj_invalidate(screen);
    }
    return true;
}

void UiConfig::applyNow(const UiConfiguration &configuration, bool notify)
{
    current_ = configuration;
    ++current_.revision;
    applyBaseDirection();

    if (notify)
    {
        emitSignal("configurationChanged");
        StackView::getInstance().cancelRootAnimations();
        StackView::getInstance().notifyConfigurationChanged(current_);
        if (lv_obj_t *screen = lv_screen_active())
        {
            lv_obj_invalidate(screen);
        }
    }
}

} // namespace lvglpp::sys
