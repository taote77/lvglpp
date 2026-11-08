# ESP32 port (scaffold)

This directory contains the ESP-IDF side of lvglpp:

- `include/lvglpp/esp/EspHardware.h` — `DisplayAdapter` / `TouchAdapter`
  interfaces. The SDK never includes st77916/cst816 vendor headers.
- `EspPlatform.{h,cpp}` — `lvglpp::sys::Platform` implementation: LVGL
  display/indev, `lv_tick_set_cb(esp_timer)`, partial draw buffers, rotation.
- `sdkconfig.defaults` — RTTI/exceptions/PSRAM/FreeRTOS baseline.
- `lv_conf_esp32.h` — additive LVGL config (RGB565, libc malloc, no
  ThorVG/Lottie/FFmpeg/FreeType/GIF/QRCode). It includes the desktop
  `sdk/lv_conf.h` and overrides only the MCU-specific values.
- `CMakeLists.txt` — IDF component template (not part of the desktop build).

## Integration

1. Add the LVGL and lvglpp components to the IDF project:

   ```cmake
   set(EXTRA_COMPONENT_DIRS
       "${CMAKE_CURRENT_LIST_DIR}/../../sdk/lvgl"
       "${CMAKE_CURRENT_LIST_DIR}")
   ```

2. Implement the two adapters in the application component:

   ```cpp
   class St77916Display final : public lvglpp::esp::DisplayAdapter { ... };
   class Cst816Touch   final : public lvglpp::esp::TouchAdapter { ... };
   ```

3. Mount the embedded asset pack before `Application::initialize()` (or in
   `onInit()`). With `target_add_binary_data(common.pak BINARY)`:

   ```cpp
   extern const std::uint8_t common_pak_start[] asm("_binary_common_pak_start");
   extern const std::uint8_t common_pak_end[]   asm("_binary_common_pak_end");

   lvglpp::tools::AssetManager::getInstance().mountMemoryView(
       common_pak_start, static_cast<std::size_t>(common_pak_end - common_pak_start));
   ```

   `mountMemoryView()` is zero-copy; the flash array must stay alive for the
   lifetime of the mount. `mountPack("path.pak")` also works on SPIFFS.

4. Start either the full Application or a widget-only loop:

   ```cpp
   auto platform = std::make_unique<lvglpp::esp::EspPlatform>(
       std::make_unique<St77916Display>(),
       std::make_unique<Cst816Touch>());
   lvglpp::sys::Application app(std::move(platform));
   app.initialize();
   return app.exec();
   ```

## Concrete board integration

`adapters/smart_speaker/` contains a ready-to-adapt implementation for the
existing `smart-speaker` ESP32-S3 project (ST77916 360x360 QSPI + CST816S,
managed LVGL 9.5). The same board's face + candle UI is available as the
reusable `demo/face_ui/` example (`FacePanel` accepts voice state and mic
level from the application). See `adapters/smart_speaker/README.md` for the component
CMake, RTTI/exception requirement, static draw buffers and main-loop wiring.

## Status and known work

- This directory is isolated from the desktop CMake build. ESP support is
  additive: Linux keeps its filesystem/mmap/FreeType/ThorVG/FFmpeg/evdev/
  prefetch/automation capabilities. See `docs/ESP32_PORTING.md` §0.
- The log backend for ESP-IDF is implemented in
  `sdk/core/log/log_esp.h` and selected by `ESP_PLATFORM`.
- `AssetManager` / `ImageManager` / `AssetPack` now have `ESP_PLATFORM`
  branches: embedded/SPIFFS pack (`mountMemoryView`, `mountPack`), no mmap or
  filesystem directory source, synchronous prefetch. The Linux implementations
  are unchanged. See `docs/ESP32_PORTING.md` §2.7 for the status table.
- Fonts must be generated as an ESP profile with
  `sdk/tools/i18n/gen_fonts_profile.py`; do not add
  `sdk/core/tools/fonts/*.c` wholesale. See `docs/ESP32_PORTING.md` §4.
  Register the generated table before building the UI:

  ```cpp
  #include "core/tools/fonts/LvFontManager.h"
  #include "font_table_zh_cn.h"

  lvglpp::tools::LvFontManager::setProfile(lvglpp_fonts_zh_cn,
                                           lvglpp_fonts_zh_cn_count);
  ```

  Without a registered profile, the ESP build falls back to
  `LV_FONT_DEFAULT` (Montserrat), which only covers the built-in glyphs.
- The st77916/cst816 adapters require the actual driver headers; this repo
  does not contain them. Provide the driver APIs or implement the adapters in
  your application project.
