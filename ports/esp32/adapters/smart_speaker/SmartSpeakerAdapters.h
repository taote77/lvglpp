#ifndef LVGLPP_SMART_SPEAKER_ADAPTERS_H
#define LVGLPP_SMART_SPEAKER_ADAPTERS_H

#include "lvglpp/esp/EspPlatform.h"

#include <memory>

namespace lvglpp::esp {

/**
 * Adapters for the smart-speaker board:
 *   ESP32-S3 + Waveshare 360x360 ST77916 (QSPI) + CST816S touch.
 *
 * The display adapter calls EXIO_Init() and LCD_Init() itself; LCD_Init()
 * already runs Touch_Init()/cst816_init() on the shared I2C bus, so the touch
 * adapter's init() is a no-op.
 *
 * Draw buffers are static .bss arrays (360 x 20 RGB565) because internal DMA
 * RAM is scarce on this board. Touch scroll limit is 40 px (tap jitter must
 * not turn a click into a drag) and the indev read period is 10 ms, matching
 * the existing main.cpp.
 *
 * Usage (widget-only integration):
 *
 *   lv_init();
 *   auto platform = makeSmartSpeakerPlatform();
 *   platform->init();
 *   ui::init();                       // existing ui_menu
 *   while (true) { lv_timer_handler(); vTaskDelay(pdMS_TO_TICKS(10)); }
 *
 * Or pass the platform to lvglpp::sys::Application and let it own lv_init()
 * and the main loop.
 */
std::unique_ptr<sys::Platform> makeSmartSpeakerPlatform();

/// Map lvglpp Label sizes to the Montserrat faces enabled in sdkconfig.
/// Call this before creating lvglpp Label widgets if you do not use the
/// factory above (the factory calls it for you).
void installSmartSpeakerFontProfile();

} // namespace lvglpp::esp

#endif // LVGLPP_SMART_SPEAKER_ADAPTERS_H
