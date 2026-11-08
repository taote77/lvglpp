#include "lvglpp/esp/EspPlatform.h"

#include <algorithm>
#include <cstddef>

#include <esp_heap_caps.h>
#include <esp_timer.h>

namespace lvglpp::esp {

namespace {

std::uint32_t tickSource()
{
    return static_cast<std::uint32_t>(esp_timer_get_time() / 1000);
}

void *allocateDrawBuffer(std::size_t bytes, bool preferPsram)
{
#if defined(MALLOC_CAP_SPIRAM)
    if (preferPsram)
    {
        if (void *memory = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT))
        {
            return memory;
        }
    }
#else
    (void)preferPsram;
#endif

    // Panel DMA normally requires internal, DMA-capable RAM.
    if (void *memory = heap_caps_malloc(bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT))
    {
        return memory;
    }
    return heap_caps_malloc(bytes, MALLOC_CAP_8BIT);
}

lv_display_rotation_t toLvglRotation(sys::Orientation orientation)
{
    switch (orientation)
    {
    case sys::Orientation::Portrait:
        return LV_DISPLAY_ROTATION_90;
    case sys::Orientation::ReverseLandscape:
        return LV_DISPLAY_ROTATION_180;
    case sys::Orientation::ReversePortrait:
        return LV_DISPLAY_ROTATION_270;
    case sys::Orientation::Landscape:
    default:
        return LV_DISPLAY_ROTATION_0;
    }
}

} // namespace

EspPlatform::EspPlatform(std::unique_ptr<DisplayAdapter> display,
                         std::unique_ptr<TouchAdapter>   touch,
                         EspPlatformConfig               config) :
    display_adapter_(std::move(display)), touch_adapter_(std::move(touch)), config_(config)
{}

EspPlatform::~EspPlatform()
{
    shutdown();
}

bool EspPlatform::init()
{
    if (initialized_)
    {
        return true;
    }
    if (!display_adapter_ || !touch_adapter_)
    {
        return false;
    }
    if (!display_adapter_->init())
    {
        return false;
    }
    if (!touch_adapter_->init())
    {
        display_adapter_->shutdown();
        return false;
    }

    lv_tick_set_cb(&tickSource);

    const int width  = display_adapter_->width();
    const int height = display_adapter_->height();
    if (width <= 0 || height <= 0)
    {
        touch_adapter_->shutdown();
        display_adapter_->shutdown();
        return false;
    }

    display_ = lv_display_create(width, height);
    if (display_ == nullptr)
    {
        touch_adapter_->shutdown();
        display_adapter_->shutdown();
        return false;
    }

    const lv_color_format_t colorFormat = display_adapter_->colorFormat();
    lv_display_set_color_format(display_, colorFormat);

    const std::uint32_t lines = std::max<std::uint32_t>(1, std::min<std::uint32_t>(config_.drawBufferLines, static_cast<std::uint32_t>(height)));
    // Rotation can swap width/height; size the partial buffer for the larger
    // dimension so LVGL never writes past it in portrait.
    const std::size_t maxWidth    = static_cast<std::size_t>(std::max(width, height));
    std::size_t       bufferBytes = maxWidth * lines * lv_color_format_get_size(colorFormat);

    if (config_.drawBuffer1 != nullptr && config_.drawBuffer2 != nullptr)
    {
        if (config_.drawBufferSize < bufferBytes)
        {
            lv_display_delete(display_);
            display_ = nullptr;
            touch_adapter_->shutdown();
            display_adapter_->shutdown();
            return false;
        }
        buf1_        = config_.drawBuffer1;
        buf2_        = config_.drawBuffer2;
        bufferBytes  = config_.drawBufferSize;
        ownsBuffers_ = false;
    } else
    {
        buf1_ = allocateDrawBuffer(bufferBytes, config_.drawBufferInPsram);
        buf2_ = allocateDrawBuffer(bufferBytes, config_.drawBufferInPsram);
        ownsBuffers_ = true;
        if (buf1_ == nullptr || buf2_ == nullptr)
        {
            releaseDrawBuffers();
            lv_display_delete(display_);
            display_ = nullptr;
            touch_adapter_->shutdown();
            display_adapter_->shutdown();
            return false;
        }
    }

    lv_display_set_buffers(display_, buf1_, buf2_, static_cast<std::uint32_t>(bufferBytes), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display_, &EspPlatform::flushCallback);
    lv_display_set_user_data(display_, this);

    indev_ = lv_indev_create();
    if (indev_ == nullptr)
    {
        releaseDrawBuffers();
        lv_display_delete(display_);
        display_ = nullptr;
        touch_adapter_->shutdown();
        display_adapter_->shutdown();
        return false;
    }
    lv_indev_set_type(indev_, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_, &EspPlatform::touchReadCallback);
    lv_indev_set_user_data(indev_, this);
    lv_indev_set_display(indev_, display_);

    if (config_.indevScrollLimit > 0)
    {
        lv_indev_set_scroll_limit(indev_, config_.indevScrollLimit);
    }
    if (config_.indevReadPeriodMs > 0)
    {
        lv_timer_t *readTimer = lv_indev_get_read_timer(indev_);
        if (readTimer != nullptr)
        {
            lv_timer_set_period(readTimer, config_.indevReadPeriodMs);
        }
    }

    initialized_ = true;
    return true;
}

void EspPlatform::shutdown()
{
    if (!initialized_)
    {
        return;
    }
    initialized_ = false;

    // LVGL owns display_/indev_; lv_deinit() deletes them after this call.
    // The draw buffers are caller-owned and can be released once the UI loop
    // has stopped.
    if (touch_adapter_)
    {
        touch_adapter_->shutdown();
    }
    if (display_adapter_)
    {
        display_adapter_->shutdown();
    }
    releaseDrawBuffers();
    indev_   = nullptr;
    display_ = nullptr;
}

bool EspPlatform::supportsPanelRotation() const noexcept
{
    return display_adapter_ != nullptr && display_adapter_->supportsRotation();
}

bool EspPlatform::applyOrientation(sys::Orientation orientation)
{
    if (display_ == nullptr)
    {
        return false;
    }

    const std::uint16_t degrees = static_cast<std::uint16_t>(orientation);

    if (display_adapter_ != nullptr && display_adapter_->supportsRotation() && display_adapter_->setRotation(degrees))
    {
        // Hardware MADCTL rotation does not change LVGL's logical resolution,
        // so swap it explicitly for portrait orientations.
        const bool portrait = orientation == sys::Orientation::Portrait || orientation == sys::Orientation::ReversePortrait;
        const int  width    = display_adapter_->width();
        const int  height   = display_adapter_->height();
        lv_display_set_resolution(display_, portrait ? height : width, portrait ? width : height);
        return true;
    }

    lv_display_set_rotation(display_, toLvglRotation(orientation));
    return true;
}

void EspPlatform::flushCallback(lv_display_t *display, const lv_area_t *area, uint8_t *pxMap)
{
    auto *self = static_cast<EspPlatform *>(lv_display_get_user_data(display));
    if (self == nullptr || self->display_adapter_ == nullptr)
    {
        lv_display_flush_ready(display);
        return;
    }

    // true: the adapter owns the DMA transfer and will call flush_ready().
    if (!self->display_adapter_->flush(display, area, pxMap))
    {
        lv_display_flush_ready(display);
    }
}

void EspPlatform::touchReadCallback(lv_indev_t *indev, lv_indev_data_t *data)
{
    auto *self = static_cast<EspPlatform *>(lv_indev_get_user_data(indev));
    if (self == nullptr || self->touch_adapter_ == nullptr)
    {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    self->touch_adapter_->read(data);
}

void EspPlatform::releaseDrawBuffers()
{
    if (!ownsBuffers_)
    {
        buf1_ = nullptr;
        buf2_ = nullptr;
        return;
    }
    if (buf1_ != nullptr)
    {
        heap_caps_free(buf1_);
        buf1_ = nullptr;
    }
    if (buf2_ != nullptr)
    {
        heap_caps_free(buf2_);
        buf2_ = nullptr;
    }
    ownsBuffers_ = false;
}

} // namespace lvglpp::esp
