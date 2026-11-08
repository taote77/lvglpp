#include "SmartSpeakerAdapters.h"

#include "core/tools/fonts/LvFontManager.h"

#include "ST77916.h"      /* EXIO_Init is in TCA9554PWR.h, included by this */
#include "cst816_touch.h"

#include <cstdint>

namespace lvglpp::esp {

namespace {

lv_display_t *g_display = nullptr;

void onColorTransferDone()
{
    if (g_display != nullptr)
    {
        lv_display_flush_ready(g_display);
    }
}

class SmartSpeakerDisplayAdapter final : public DisplayAdapter
{
public:
    bool init() override
    {
        // EXIO_Init() is not idempotent; the application may already have
        // created the shared I2C bus for audio/backlight.
        if (EXIO_get_i2c_bus() == nullptr)
        {
            EXIO_Init();
        }
        // LCD_Init() also runs Touch_Init() -> cst816_init(). Skip it if the
        // application already initialized the panel.
        if (panel_handle == nullptr)
        {
            LCD_Init();
        }
        lcd_set_color_done_cb(&onColorTransferDone);
        return panel_handle != nullptr;
    }

    void shutdown() override
    {
        Set_Backlight(0);
    }

    int width() const override
    {
        return EXAMPLE_LCD_WIDTH;
    }

    int height() const override
    {
        return EXAMPLE_LCD_HEIGHT;
    }

    lv_color_format_t colorFormat() const override
    {
        // The project deliberately uses RGB565_SWAPPED (not LV_COLOR_16_SWAP)
        // to avoid colored anti-aliasing fringes on this panel.
        return LV_COLOR_FORMAT_RGB565_SWAPPED;
    }

    bool flush(lv_display_t *display, const lv_area_t *area, uint8_t *pxMap) override
    {
        g_display = display;
        esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1,
                                  area->y2 + 1, pxMap);
        // Async: onColorTransferDone() calls lv_display_flush_ready().
        return true;
    }

    bool supportsRotation() const override
    {
        return false;
    }
};

class SmartSpeakerTouchAdapter final : public TouchAdapter
{
public:
    bool init() override
    {
        // LCD_Init() already called cst816_init() on the shared I2C bus.
        return true;
    }

    void shutdown() override
    {}

    void read(lv_indev_data_t *data) override
    {
        // Keep the last valid point so a release does not look like a drag.
        static lv_point_t last_point   = {0, 0};
        static bool       last_pressed = false;

        uint16_t x       = 0;
        uint16_t y       = 0;
        bool     pressed = false;
        esp_err_t ret    = cst816_read(&x, &y, &pressed);

        if (ret == ESP_OK)
        {
            last_pressed = pressed;
            if (pressed)
            {
                last_point.x = x;
                last_point.y = y;
            }
        }
        // On an I2C error keep the previous state; otherwise a transient bus
        // failure would be reported as a release and cancel the click.

        data->point.x = last_point.x;
        data->point.y = last_point.y;
        data->state   = last_pressed ? LV_INDEV_STATE_PRESSED
                                     : LV_INDEV_STATE_RELEASED;
    }
};

} // namespace

void installSmartSpeakerFontProfile()
{
    static const LvglppFontFace faces[] = {
        {14, LV_FONT_DEFAULT, LV_FONT_DEFAULT},
#if LV_FONT_MONTSERRAT_16
        {16, &lv_font_montserrat_16, &lv_font_montserrat_16},
#endif
#if LV_FONT_MONTSERRAT_20
        {20, &lv_font_montserrat_20, &lv_font_montserrat_20},
#endif
#if LV_FONT_MONTSERRAT_24
        {24, &lv_font_montserrat_24, &lv_font_montserrat_24},
#endif
#if LV_FONT_MONTSERRAT_48
        {48, &lv_font_montserrat_48, &lv_font_montserrat_48},
#endif
    };
    tools::LvFontManager::setProfile(faces, sizeof(faces) / sizeof(faces[0]));
}

std::unique_ptr<sys::Platform> makeSmartSpeakerPlatform()
{
    // Static .bss draw buffers: internal DMA RAM is scarce on this board.
    // 360 x 20 RGB565 = 14400 bytes each.
    alignas(4) static uint16_t s_buf1[EXAMPLE_LCD_WIDTH * 20];
    alignas(4) static uint16_t s_buf2[EXAMPLE_LCD_WIDTH * 20];

    EspPlatformConfig config;
    config.drawBuffer1    = s_buf1;
    config.drawBuffer2    = s_buf2;
    config.drawBufferSize = sizeof(s_buf1);
    config.indevScrollLimit  = 40;
    config.indevReadPeriodMs = 10;

    installSmartSpeakerFontProfile();

    return std::make_unique<EspPlatform>(
        std::make_unique<SmartSpeakerDisplayAdapter>(),
        std::make_unique<SmartSpeakerTouchAdapter>(), config);
}

} // namespace lvglpp::esp
