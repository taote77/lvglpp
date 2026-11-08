#ifndef LVGLPP_ESP_HARDWARE_H
#define LVGLPP_ESP_HARDWARE_H

#include <cstddef>
#include <cstdint>

#include <lvgl.h>

namespace lvglpp::esp {

/**
 * Display adapter for ESP-IDF.
 *
 * Implement this in the application project around the existing st77916
 * driver. The SDK intentionally does not include vendor driver headers.
 */
class DisplayAdapter
{
public:
    virtual ~DisplayAdapter() = default;

    virtual bool init()     = 0;
    virtual void shutdown() = 0;

    virtual int width() const  = 0;
    virtual int height() const = 0;

    /// RGB565 by default. Use LV_COLOR_FORMAT_RGB565_SWAPPED when the panel
    /// expects the opposite byte order.
    virtual lv_color_format_t colorFormat() const
    {
        return LV_COLOR_FORMAT_RGB565;
    }

    /**
     * Start (or perform) one flush.
     *
     * Return false when the transfer is already complete; EspPlatform then
     * calls lv_display_flush_ready(). Return true when the adapter takes
     * ownership of an asynchronous DMA transfer and will call
     * lv_display_flush_ready(display) from its completion callback.
     */
    virtual bool flush(lv_display_t *display, const lv_area_t *area, uint8_t *pxMap) = 0;

    /// Hardware MADCTL rotation. Return false to let LVGL rotate in software.
    virtual bool supportsRotation() const
    {
        return false;
    }

    virtual bool setRotation(std::uint16_t degrees)
    {
        (void)degrees;
        return false;
    }
};

/// Touch adapter, typically wrapping cst816_touch over I2C.
class TouchAdapter
{
public:
    virtual ~TouchAdapter() = default;

    virtual bool init()     = 0;
    virtual void shutdown() = 0;

    /**
     * Non-blocking read from the LVGL indev callback. Update data->point and
     * data->state when a sample is available; leave them untouched otherwise.
     */
    virtual void read(lv_indev_data_t *data) = 0;
};

struct EspPlatformConfig {
    /// Lines per draw buffer when EspPlatform allocates them. 20 lines at
    /// 360 px RGB565 is ~14 KB per buffer (the smart-speaker board uses this).
    std::uint32_t drawBufferLines = 20;

    /// Prefer PSRAM for the draw buffers. Usually false: panel DMA needs
    /// internal RAM. Use PSRAM for decoded images instead.
    bool drawBufferInPsram = false;

    /// Optional caller-owned draw buffers (e.g. static uint16_t arrays in
    /// .bss). When both are set and drawBufferSize is large enough,
    /// EspPlatform uses them instead of allocating. They are not freed on
    /// shutdown.
    void       *drawBuffer1    = nullptr;
    void       *drawBuffer2    = nullptr;
    std::size_t drawBufferSize = 0;

    /// 0 keeps LVGL's default. The CST816 board uses 40 px / 10 ms so tap
    /// jitter is not classified as a drag and does not swallow clicks.
    std::uint16_t indevScrollLimit  = 0;
    std::uint16_t indevReadPeriodMs = 0;
};

} // namespace lvglpp::esp

#endif // LVGLPP_ESP_HARDWARE_H
