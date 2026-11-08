# smart-speaker 集成指南

目标工程：`C:\workspace\esp32\smart-speaker`（ESP32-S3，ST77916 360×360 QSPI，
CST816S，ESP-IDF 6.2，LVGL 9.5 managed component）。

本目录提供：

- `SmartSpeakerAdapters.h/.cpp`：把现有 `st77916_driver` / `cst816_touch`
  适配成 `lvglpp::esp::DisplayAdapter` / `TouchAdapter`。
- `makeSmartSpeakerPlatform()`：返回配置好的 `EspPlatform`（静态 draw
  buffer、40px scroll limit、10ms 触摸轮询、RGB565_SWAPPED）。
- `installSmartSpeakerFontProfile()`：把 lvglpp `Label` 字号映射到工程已启用的
  Montserrat 16/20/24/48。
- 圆脸 + 吹蜡烛 UI 已抽成独立示例：`demo/smart_speaker/`。ESP32 工程可以直接
  复用 `FacePanel`，由语音管线调用 `setVoiceState()` / `setMicLevel()`。

## 1. 关键差异（与桌面/通用 ESP 配置）

| 项 | smart-speaker 做法 | 适配方式 |
|---|---|---|
| LVGL 来源 | managed `lvgl/lvgl` 9.5.0 | lvglpp component `REQUIRES lvgl`，**不要再加 `sdk/lvgl`** |
| 颜色格式 | `LV_COLOR_FORMAT_RGB565_SWAPPED` | adapter 的 `colorFormat()` 返回它 |
| draw buffer | 静态 `uint16_t[360*20]` × 2（内部 DMA RAM 紧张） | `EspPlatformConfig::drawBuffer1/2` + `drawBufferSize` |
| flush | `esp_lcd_panel_draw_bitmap`，DMA 完成回调里 `flush_ready` | adapter `flush()` 返回 `true`，回调调用 `lv_display_flush_ready()` |
| 触摸 | `cst816_read()`，原始坐标已是 0..359（**不要再 ×1.5**），抬手保持最后点 | `SmartSpeakerTouchAdapter` |
| scroll limit | 40px（避免触摸抖动把点击判成拖动） | `indevScrollLimit = 40` |
| 触摸轮询 | 10ms | `indevReadPeriodMs = 10` |
| tick | esp_timer 1ms → `lv_tick_inc(1)` | `EspPlatform` 用 `lv_tick_set_cb(esp_timer_get_time)`，可删掉原 timer |
| C++ 运行时 | RTTI/异常 **关闭** | 必须开启，见 §2 |
| LVGL 日志 | `CONFIG_LV_USE_LOG` 未开启 | 不影响；lvglpp 自己的日志走 `log_esp.h` |

## 2. 必须开启 RTTI / 异常

lvglpp 使用 `typeid`、`dynamic_cast`、`std::any_cast` + `try/catch`，
`base64.cpp` 还会 `throw`。当前工程的 `sdkconfig` 是：

```text
# CONFIG_COMPILER_CXX_EXCEPTIONS is not set
# CONFIG_COMPILER_CXX_RTTI is not set
```

在 `sdkconfig.defaults` 增加：

```text
CONFIG_COMPILER_CXX_EXCEPTIONS=y
CONFIG_COMPILER_CXX_RTTI=y
```

然后按工程 README 的说明删除根目录 `sdkconfig` 再构建（否则 defaults 不生效）。
这会增加 flash；若产品不能接受，需要按 `docs/ESP32_PORTING.md` §2.1 做
去 RTTI/异常重构。

## 3. 把 lvglpp 放进工程

推荐把 lvglpp 检出到 Windows 可访问的目录，例如
`C:\workspace\esp32\smart-speaker\components\lvglpp\lvglpp`，然后新增
`components/lvglpp/CMakeLists.txt`：

```cmake
set(LVGLPP_ROOT "${CMAKE_CURRENT_LIST_DIR}/lvglpp" CACHE PATH "lvglpp checkout")
set(LVGLPP_SDK  "${LVGLPP_ROOT}/sdk")

file(GLOB_RECURSE LVGLPP_SOURCES
    "${LVGLPP_SDK}/core/base/*.cpp"
    "${LVGLPP_SDK}/core/widgets/*.cpp"
    "${LVGLPP_SDK}/core/style/*.cpp"
    "${LVGLPP_SDK}/core/sys/*.cpp"
    "${LVGLPP_SDK}/core/tools/*.cpp"
)
list(APPEND LVGLPP_SOURCES
    "${LVGLPP_SDK}/config/color_sheet.cpp"
    "${LVGLPP_SDK}/core/sys/platform/PlatformFactory.cpp"
    "${LVGLPP_ROOT}/ports/esp32/EspPlatform.cpp"
)

# Desktop-only backends and optional subsystems.
list(FILTER LVGLPP_SOURCES EXCLUDE REGEX "/core/sys/platform/(Sdl|Framebuf|Drm)Platform\\.cpp$")
list(FILTER LVGLPP_SOURCES EXCLUDE REGEX "/core/sys/platform/EvdevInput\\.cpp$")
list(FILTER LVGLPP_SOURCES EXCLUDE REGEX "/core/sys/automation/")
list(FILTER LVGLPP_SOURCES EXCLUDE REGEX "/core/log/log\\.cpp$")

idf_component_register(
    SRCS ${LVGLPP_SOURCES}
    INCLUDE_DIRS
        "${LVGLPP_SDK}"
        "${LVGLPP_ROOT}/ports/esp32/include"
        "${LVGLPP_ROOT}/ports/esp32"
    REQUIRES lvgl esp_timer
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LVGLPP_PLATFORM_ESP32=1)
```

要点：

- **不要**把 `sdk/lvgl` 加进 `EXTRA_COMPONENT_DIRS`；工程已经有 managed LVGL。
- **不要**设置 `LV_CONF_PATH`/`LV_KCONFIG_IGNORE`；managed LVGL 使用工程的
  `sdkconfig` Kconfig。
- 只编译 `sdk/core`，不要 glob `sdk/core/tools/fonts/*.c`；ESP 下
  `LvFontManager` 会回退 `LV_FONT_DEFAULT`，字库由 §5 的 profile 提供。
- 如果只想先做 Widget 层，也可以只编译 `base/widgets/style/sys/tools` 的
  便携子集；`Application` 不是必须的。

## 4. main 集成

### 4.1 推荐：Widget-only（保留现有主循环/服务）

把 `SmartSpeakerAdapters.h/.cpp` 复制到 `main/`，然后：

```cmake
# main/CMakeLists.txt
idf_component_register(
    SRCS "console_cmd.cpp" "ui_menu.cpp" "main.cpp" "wifi_prov.cpp"
         "SmartSpeakerAdapters.cpp"
    INCLUDE_DIRS "."
    REQUIRES lcd audio voice lvgl lvglpp bt esp_wifi esp_netif nvs_flash esp_event console
    PRIV_REQUIRES esp_timer
)
```

`main.cpp` 中：

1. 删除原来的 `g_buf1/g_buf2`、`lvgl_tick_cb`、`lcd_flush_done_cb`、
   `lvgl_flush_cb`、`touch_read_cb`、`init_display()`、`init_touch()`、
   `init_lvgl_tick()`。
2. 保留/提前 `EXIO_Init()`（音频也要用共享 I2C 总线；adapter 会检测并避免
   重复初始化）。
3. 在 `ui::init()` 之前：

```cpp
lv_init();
auto platform = lvglpp::esp::makeSmartSpeakerPlatform();
if (!platform->init())
{
    ESP_LOGE(TAG, "platform init failed");
    return;
}
ui::init();
```

4. 主循环保持：

```cpp
lv_timer_handler();          // lv_task_handler 的别名
vTaskDelay(pdMS_TO_TICKS(10));
```

`makeSmartSpeakerPlatform()` 内部的 adapter `init()`：

- 如果 `EXIO_get_i2c_bus()` 为空，调用 `EXIO_Init()`；
- 如果 `panel_handle` 为空，调用 `LCD_Init()`（其中会 `cst816_init()`）；
- 注册 `lcd_set_color_done_cb()`，DMA 完成后调用 `lv_display_flush_ready()`。

如果工程在更早的地方已经调用了 `LCD_Init()`，adapter 会跳过，不会重复初始化。

### 4.2 可选：Full Application

```cpp
class SpeakerApp final : public lvglpp::sys::Application
{
public:
    using lvglpp::sys::Application::Application;

protected:
    bool onInit() override
    {
        ui::init();
        return true;
    }
};
```

此时由 `Application` 负责 `lv_init()`、display/indev、tick、主循环和 shutdown；
删除 `main.cpp` 里手写的 display/touch/tick 代码。BLE/音频/语音服务在
`app.exec()` 之前初始化；它们的后台任务只能通过 `Application::postEvent()`
与 UI 通信。注意 `main.cpp` 里已有的 `class Application` 名字会和
`lvglpp::sys::Application` 冲突，建议重命名或使用全限定名。

### 4.3 共享圆脸/蜡烛页

`sync_to_smart_speaker.sh` 会把 lvglpp `demo/smart_speaker/` 的
`FaceTypes.h`、`FacePage.*`、`CandlePage.*` 复制到工程的
`main/smart_speaker_ui/`。`main/CMakeLists.txt` 需要：

```cmake
SRCS ... "smart_speaker_ui/FacePage.cc" "smart_speaker_ui/CandlePage.cc"
INCLUDE_DIRS "." "smart_speaker_ui"
```

`ui_menu.cpp` 直接把两个页面放进外层 TileView 的 tile0/tile1：

```cpp
face_page_ = std::make_unique<demo::FacePage>(homeTile);
face_page_->setOnPlay([this] { open_app(kTileCandle); });

candle_page_ = std::make_unique<demo::CandlePage>(candleTile);
candle_page_->setOnBack([this] { go_home(); });
```

`MenuUi` 每 100ms 根据 `tileview_->activeTile()` 调用
`FacePage::setActive()` / `CandlePage::setActive()`，暂停隐藏页的动画；
`set_voice()` / `set_mic_level()` 需要同时转发给两个页面。这样计算器、
设置等业务页仍留在原来的 TileView 中，圆脸/蜡烛只有一份源码。

## 5. 字体

工程当前启用 Montserrat 16/20/24/48。`makeSmartSpeakerPlatform()` 会调用
`installSmartSpeakerFontProfile()`，让 lvglpp `Label::FontSize*` 映射到这些
内置字体。没有 CJK 字形（当前 `ui_menu` 也没有中文）。

需要中文/多语言时，用 `sdk/tools/i18n/gen_fonts_profile.py` 生成 profile：

```bash
python3 -I sdk/tools/i18n/gen_fonts_profile.py \
  --xlsx smart_home/i18n/strings.xlsx \
  --langs zh-CN --sizes 16,24 --styles normal --bpp 4 --compress \
  --out ports/esp32/fonts --emit-table
```

把生成的 `.c` 加入 `main` 或 lvglpp component 的 SRCS，并注册：

```cpp
lvglpp::tools::LvFontManager::setProfile(lvglpp_fonts_zh_cn,
                                         lvglpp_fonts_zh_cn_count);
```

## 6. 迁移顺序建议

1. 先只编译 lvglpp core + `EspPlatform`，用现有 `ui_menu` 验证显示/触摸/tick。
2. 新页面/新控件逐步用 `Widget`/`Label`/`PushButton`/`WidgetStore` 写。
3. 需要时再引入 `Activity`/`StackView`/`PageHost`。
4. 最后迁移 `ui_menu.cpp` 的 tileview 页面；目前 SDK 没有 `TileView` 包装，
   需要先加一个 `TileView` 控件或暂时保留 `lv_tileview_*`。

## 7. 已知限制

- 本目录的 adapter 代码需要在带 `lcd` 组件的 IDF 工程中编译；当前仓库的
  Linux 构建不会编译它。
- `EspPlatform` 没有实现硬件旋转；360×360 方屏不需要。需要 90/270 时走
  LVGL 软件旋转或扩展 adapter。
- `ui_menu.cpp` 目前直接使用 `lv_tileview`/`lv_font_montserrat_*`；迁移到
  Widget API 是后续独立工作。
