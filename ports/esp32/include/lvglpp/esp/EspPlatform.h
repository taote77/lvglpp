#ifndef LVGLPP_ESP_PLATFORM_H
#define LVGLPP_ESP_PLATFORM_H

#include "core/sys/platform/Platform.h"
#include "lvglpp/esp/EspHardware.h"

#include <memory>

namespace lvglpp::esp {

/**
 * ESP-IDF backend for sys::Application.
 *
 * The Application owns lv_init()/lv_deinit() and the main loop; this class
 * only creates the LVGL display/indev, provides the tick source and drives
 * the injected hardware adapters.
 *
 * Tick: providesTick() is true and init() installs lv_tick_set_cb(), so
 * Application does not start its std::thread tick fallback.
 */
class EspPlatform final : public sys::Platform
{
public:
    EspPlatform(std::unique_ptr<DisplayAdapter> display,
                std::unique_ptr<TouchAdapter> touch,
                EspPlatformConfig config = {});
    ~EspPlatform() override;

    EspPlatform(const EspPlatform &)            = delete;
    EspPlatform &operator=(const EspPlatform &) = delete;

    const char *name() const noexcept override
    {
        return "ESP32";
    }

    bool init() override;
    void shutdown() override;

    lv_display_t *display() const override
    {
        return display_;
    }

    void setTheme(lv_theme_t *) override
    {}

    void simulatePointerClick(std::int32_t, std::int32_t) override
    {}

    bool providesTick() const noexcept override
    {
        return true;
    }

    bool supportsPanelRotation() const noexcept override;

    bool applyOrientation(sys::Orientation orientation) override;

private:
    static void flushCallback(lv_display_t *display, const lv_area_t *area, uint8_t *pxMap);
    static void touchReadCallback(lv_indev_t *indev, lv_indev_data_t *data);

    void releaseDrawBuffers();

    std::unique_ptr<DisplayAdapter> display_adapter_;
    std::unique_ptr<TouchAdapter>   touch_adapter_;
    EspPlatformConfig               config_;

    lv_display_t *display_ = nullptr;
    lv_indev_t   *indev_   = nullptr;
    void         *buf1_    = nullptr;
    void         *buf2_    = nullptr;
    bool          ownsBuffers_ = false;
    bool          initialized_ = false;
};

} // namespace lvglpp::esp

#endif // LVGLPP_ESP_PLATFORM_H
