# ESP32 / ESP-IDF 移植评审与方案

本文评审“把 lvglpp 移植到 ESP32，复用 st77916 + cst816，按需裁剪字体”的方案，并给出可落地的分阶段计划。结论先说：

- **可行**，但原方案的顺序需要调整：先做“可移植性层”（RTTI/异常、文件系统、日志、LVGL 配置），再做显示/触摸 bring-up，最后才接 Activity/StackView。
- 最大风险不是 LVGL 驱动，而是 **C++ 运行时依赖**：`typeid`/`dynamic_cast`/`std::any_cast` 需要 RTTI 和异常；`std::filesystem` 出现在 7 个核心文件里；当前字体资源约 90 MB 源码，必须按 profile 重做。
- 建议保留两条集成路径：
  1. **Widget-only**：只嵌 `core/base + widgets + style + tools`，由现有 `main.cpp` 驱动 `lv_timer_handler()`。
  2. **Full Application**：移植 `sys::Application + Platform + Activity/StackView/PageHost`，由 SDK 接管初始化和主循环。
  先做 1 验证硬件，再做 2。

---

## 0. 不降低 Linux/SoC 能力的硬约束

ESP32 支持采用**增量适配（additive port）**，而不是“为了 MCU 削减功能”：

1. `ports/esp32/` 与 `ESP_PLATFORM` 分支只影响 ESP-IDF 构建；桌面 CMake、默认编译选项和源码路径保持原样。
2. **禁止**把 Linux 已有实现用 `#ifndef ESP_PLATFORM` 包住后删除；原有代码必须保留，ESP 分支只提供**替代实现**。
3. 新能力（如 `mountMemoryView`、嵌入式 pack、Tiny TTF）是 additive：Linux 继续使用 mmap / filesystem / 热更新 / FreeType。
4. 桌面默认配置不引入 ESP 的裁剪宏；ESP 的裁剪只写在该 target 的 `lv_conf`/`sdkconfig`/component CMake 中。
5. 每个 `#ifdef ESP_PLATFORM` 都必须有对应的桌面路径，并且桌面全选项构建 + `test_unit` 保持通过。
6. 能用 `Platform` 运行时多态解决的（显示、触摸、tick、旋转），不要用编译期 guard；编译期 guard 只用于 OS 设施（filesystem/信号/线程/驱动）。

### 能力矩阵

| 能力 | Linux SDL/FB/DRM | ESP32 | 隔离方式 |
|---|---|---|---|
| SDL / FRAMEBUF / DRM 后端 | ✅ | ❌ | `Platform` 子类 + 平台宏 |
| evdev / 物理键盘输入 | ✅ | ❌（CST816） | Platform 后端 |
| 文件系统资产、目录热更新 | ✅ | ❌（embedded pack） | `AssetManager` source 后端 |
| pack `mmap` / `materialize()` | ✅ | ❌（flash memory view） | `#ifdef ESP_PLATFORM` 替代实现 |
| FreeType 矢量字体 | ✅ 可选 | ❌（位图/Tiny TTF） | `LV_USE_FREETYPE` + target 配置 |
| ThorVG / Lottie | ✅ | ❌ 默认 | `LV_USE_THORVG` + target 配置 |
| FFmpeg / `Video` | ✅ | ❌ | `LV_USE_FFMPEG` + target 配置 |
| SIGINT/SIGTERM 处理 | ✅ | ❌ | `__unix__` guard（已有） |
| `std::thread` tick 兜底 | ✅ | ❌（esp_timer） | `Platform::providesTick()` |
| 图片 prefetch worker | ✅ | ❌（同步加载） | feature guard |
| automation / EventRecorder | ✅ | ❌ | component 排除 |
| Lua 绑定 | ✅ 可选 | ❌ | component 排除 |
| `ws_gw` | ✅ | 可选（lwIP BSD socket） | 独立 component |
| 四方向旋转 / RTL | ✅ | ✅ | 同一套 `UiConfig`/`Widget` |
| Activity / StackView / PageHost | ✅ | ✅ | 同一套代码 |
| i18n（lv_i18n） | ✅ | ✅ | 同一份生成 C |
| 字体 | 全量/FreeType | 按 profile 裁剪 | 生成器参数 + target 输出目录 |
| 测试/demo | ✅ | ❌（不构建） | 桌面 CMake 不变 |

### Guard 写法约定

```cpp
// 好：保留 Linux 实现，ESP 只加替代分支
#if defined(ESP_PLATFORM)
    return mountEmbeddedPack(...);
#else
    return mountPackFromFilesystem(...);
#endif

// 不好：把 Linux 能力裁掉
#ifndef ESP_PLATFORM
    ... // Linux only, ESP 下整个功能消失且无替代
#endif

// 更好：能用运行时多态就不要编译期分支
platform_->applyOrientation(orientation);
```

Phase 0 的验收标准必须包含：**桌面端所有现有构建组合（SDL/FRAMEBUF/DRM、FreeType ON/OFF、rotation/RTL ON/OFF、Lua ON、ASAN）和 `test_unit` 全部通过**，再开始 ESP 侧功能开发。

---

## 1. 对原方案的逐项评审

| 原方案 | 结论 | 需要修正的点 |
|---|---|---|
| 字体按需裁剪 | ✅ 方向正确 | 现有的 `gen_fonts.py` 已经按 `strings.xlsx` 裁剪，但仍是 **5 种语言 × 6 字号 × 2 字重 × 8bpp × 不压缩**，24 个 `.c` 共约 90 MB 源码。ESP32 必须按“语言 + 字号 + 字重 + bpp + 压缩”生成 profile。见 §4。 |
| ESP-IDF 版 `core/log/log.h` | ✅ 可行，已落地 | 已新增 `framework/core/log/log_esp.h`，`log.h` 在 `ESP_PLATFORM` 下切换；保持 `LOG_DEBUG() << ...` 调用不变，避免 `<filesystem>/<fstream>/<iostream>`。见 §3。 |
| `EspPlatform : sys::Platform` 复用 st77916/cst816 | ✅ 可行 | **不要在 SDK 里 include 应用的驱动头**。用 `DisplayAdapter`/`TouchAdapter` 注入，应用侧写两个薄适配器包住现有驱动。见 §5。 |
| tick 线程换 esp_timer/FreeRTOS task | ✅ 可行，但不需要改 Application | `Platform::providesTick()` 已存在：`EspPlatform` 返回 `true`，在 `init()` 里 `lv_tick_set_cb([]{ return esp_timer_get_time()/1000; })`，`Application` 就不会启动 `std::thread`。 |
| `std::filesystem/signal/ThorVG` 用 `#ifndef ESP_PLATFORM` 裁掉 | ⚠️ 部分已存在，但范围被低估 | `signal` 已由 `__unix__ || __APPLE__` 守卫；`Application.cpp` 的 `#include "thorvg.h"` 已有 `#if LV_USE_THORVG` 守卫，ESP target 必须把 `LV_USE_THORVG` 配成 0；`std::filesystem` 涉及 7 个文件，不能只加一个总开关。见 §2。 |
| Application 接管 vs 只嵌 Widget 层 | ✅ 两条都合理 | 建议先 Widget-only，再 Full Application。见 §6 的 Phase 1/2。 |
| Phase 3 移植 Activity/StackView/InputManager/PageHost | ✅ 基本可移植 | `InputManager` 没有 Linux 依赖；但 `Activity`/`BaseActivity`/`Widget` 使用 `typeid`，`Widget::findChildrenByType` 使用 `dynamic_cast`，`Event::convertData` 使用 `std::any_cast` + `try/catch`。ESP-IDF 必须开启 RTTI/异常，或先重构。 |
| LvFontManager/ImageManager/AssetManager 改内嵌数组或 SPIFFS | ✅ 正确，但优先内嵌 | 只读资源用 **embedded pack + 零拷贝 memory view** 比 SPIFFS 更省 RAM/CPU；SPIFFS 只用于需要运行时可更新的资源。`ImageManager::loadFile()`/prefetch 线程在 ESP 上裁掉。 |
| 最后迁移 `ui_menu.cpp` | ✅ 顺序正确 | 先迁移到 `Widget/WidgetStore`，再替换回调；不要在迁移前把 `lv_obj_t*` 长期保存在应用结构里。 |

---

## 2. 可移植性阻断点（按严重程度）

### 2.1 RTTI / 异常（必须先决策）

| 位置 | 用法 | 影响 |
|---|---|---|
| `sys/Activity.cpp` | `typeid(*this).name()` | 需要 RTTI |
| `sys/BaseActivity.cpp` | 3 处 `typeid` 日志 | 需要 RTTI |
| `widgets/Widget.cpp` | `typeid(*wrapper).name()`、`dynamic_cast<T*>` | 需要 RTTI |
| `sys/automation/EventRecorder.cpp` | `typeid` | 可整目录排除 |
| `sys/Event.h` | `std::any_cast<T>(data_)` + `catch (std::bad_any_cast)` | 需要异常；或改成 `std::any_cast<T>(&data_)` |
| `tools/base64.cpp` | `throw std::runtime_error` | 需要异常；或改成返回 `bool` |
| `tools/Utils.h` | `catch (std::exception&)` | 需要异常 |

两条路：

**A. 打开 ESP-IDF 的 C++ 运行时（最小改动，推荐先做）**

```text
CONFIG_COMPILER_CXX_RTTI=y
CONFIG_COMPILER_CXX_EXCEPTIONS=y
```

代价是 flash/异常表增大。若产品接受，这是最快路径。

**B. 去掉 RTTI/异常（更省 flash，但要改 SDK）**

- `Event::convertData` 改用指针形式 `std::any_cast<T>(&data_)`，去掉 try/catch。
- `base64.cpp` 改为返回 `bool`。
- `Activity::name()` 改为虚函数/显式字符串；`BaseActivity` 日志去掉 `typeid`。
- `Widget::findChildrenByType` 用类型标签/注册表替代 `dynamic_cast`。
- 排除 `sys/automation/*`。

建议：**Phase 0 先用 A 打通，Phase 4 再评估 B**。

### 2.2 `std::filesystem`（7 个文件）

| 文件 | 用途 | ESP32 处理 |
|---|---|---|
| `sys/Application.cpp` | `current_path()` 作为资产根 | 用 `LVGLPP_ASSET_DIR`/内嵌资源替代；`#ifndef ESP_PLATFORM` 分支 |
| `sys/platform/PlatformFactory.cpp` | `argv[0]` stem | ESP 用 `EspPlatform` 工厂，不解析 argv |
| `log/log.h` / `log/log.cpp` | 文件日志 | 已切换 `log_esp.h`，ESP 下不编译 `log.cpp` |
| `tools/AssetManager.cpp` | DirectorySource、materialize、temp 目录 | 只保留 Pack/memory source；新增 `mountMemoryView` |
| `tools/ImageManagerImpl.cpp` | `loadFile()` 绝对路径 key | `#ifndef ESP_PLATFORM` 裁掉 `loadFile` |
| `tools/assets/AssetPack.h` | respack 写包 | 拆分“运行时解析”和“打包 CLI”；或整段 guard |

### 2.3 POSIX 依赖

- `AssetManager.cpp`：`open/fstat/mmap/getpid`、`<unistd.h>`、`<fcntl.h>`、`<sys/mman.h>`。
  → ESP 上改为 embedded pack：把 `common.pak` 通过 `target_add_binary_data` 编译成 `const uint8_t[]`，用零拷贝 `mountMemoryView()` 挂载；不使用 mmap。
- `tools/AssetPack.h` 的 `std::filesystem::rename` 等只属于打包工具，运行时不需要。
- `sys/platform/{Sdl,Framebuf,Drm}Platform.cpp`、`EvdevInput.cpp`：ESP component 直接排除。

### 2.4 线程 / 定时器

| 位置 | 现状 | ESP32 处理 |
|---|---|---|
| `Application` tick | `std::thread` 每 5 ms `lv_tick_inc(5)` | `EspPlatform::providesTick() == true` + `lv_tick_set_cb(esp_timer_get_time)` |
| `Application::exec` | `std::this_thread::sleep_for` | ESP 下可用 `vTaskDelay(pdMS_TO_TICKS(delay))`；不改也能跑（std::thread 在 IDF 可用），但更省 flash |
| `ImageManagerImpl` prefetch | `std::thread` worker | ESP 上 `#ifndef ESP_PLATFORM` 裁掉 prefetch；同步 `load()` 足够 |
| `automation/EventRecorder`、`AutomationController` | `std::thread` | 整目录排除 |
| `base64`/`Utils` | 异常 | 见 §2.1 |

### 2.5 LVGL 配置

当前 `framework/lv_conf.h` 是桌面模拟器配置，ESP32 必须覆盖：

| 宏 | 当前 | ESP32 建议 |
|---|---|---|
| `LV_COLOR_DEPTH` | 32 | 16（ST77916 RGB565） |
| `LV_MEM_SIZE` | 5 MB（builtin） | `LV_STDLIB_CLIB` + 64–256 KB，或用 PSRAM 分配 |
| `LV_USE_OS` | `LV_OS_NONE` | 单 LVGL 任务保持 `NONE`；不要多任务并发访问 LVGL |
| `LV_USE_THORVG_INTERNAL` | 1 | 0（除非必须 Lottie） |
| `LV_USE_LOTTIE` | 1 | 0 |
| `LV_USE_FFMPEG` | 1 | 0 |
| `LV_USE_FREETYPE` | 0 | 0（ESP 用位图/Tiny TTF） |
| `LV_USE_DEMO_EBIKE` | 1 | 0 |
| `LV_USE_SDL/FBDEV/DRM/EVDEV` | 随平台 | 全部 0 |
| `LV_DRAW_TRANSFORM_USE_MATRIX` | 旋转时开启 | 若不需要四方向旋转，关闭 |
| `LV_USE_GIF` / `LV_USE_QRCODE` | 1 | 按需；QR 码有 flash 成本 |
| `LV_DEF_REFR_PERIOD` | 33 ms | 按面板/动画需求调整 |

已提供 `ports/esp32/lv_conf_esp32.h`：它 include 桌面 `framework/lv_conf.h` 后只覆盖 MCU 相关项。`ports/esp32/CMakeLists.txt` 通过 `LV_CONF_PATH` + `LV_KCONFIG_IGNORE=1` 把它同时应用到 `lvgl` 和 `lvglpp` target；桌面构建永远不会包含它。

如果更倾向 LVGL 的 Kconfig，也可以改用 `CONFIG_LV_CONF_SKIP=y` + `CONFIG_LV_*`，但不要在同一个构建里同时启用两套配置源。

不要复用桌面 `framework/lv_conf.h`，它的 800×480/32bpp/5MB/ThorVG 假设会直接把 RAM 吃光。

### 2.6 其他

- `smart_home` 不要整体移植：它使用 C++20 `<format>`、5 个 tab、800×480 布局和完整 i18n 字体。ESP32 应用应只依赖 SDK + 你的 `ui_menu`。
- `PlatformFactory.cpp` 的 `executableStem()` 用 filesystem；ESP 用独立工厂。
- `FileSystemManager` 的 `A:` 驱动本身是 LVGL FS 抽象，可保留。
- `ws_gw` 使用 POSIX socket + `std::thread`；ESP-IDF lwIP 有 BSD socket，可作为可选组件，不要放进第一阶段。

---

### 2.7 当前实现状态

| 项目 | 状态 |
|---|---|
| `log.h` / `log.cpp` / `log_esp.h` | ✅ ESP 分支已实现，桌面路径未变 |
| `Application.{h,cpp}` | ✅ filesystem / tick thread / FreeRTOS sleep 分支 |
| `PlatformFactory.{h,cpp}` | ✅ `PlatformKind::Esp32`、filesystem guard、ESP 工厂提示 |
| `AssetManager.{h,cpp}` | ✅ `mountMemoryView()`、ESP 无 mmap/目录源、SPIFFS pack 读取 |
| `ImageManagerImpl.{h,cpp}` | ✅ ESP 无 prefetch 线程、filesystem 分支移除 |
| `AssetPack.h` | ✅ writer（filesystem）在 ESP 下不编译，reader 保留 |
| `LvFontManager.cpp` | ✅ ESP 下回退 `LV_FONT_DEFAULT`；`setProfile()` 已接入生成表 |
| `gen_fonts_profile.py` | ✅ 已实现并实测（per-language / bpp / compress / table） |
| `ports/esp32/lv_conf_esp32.h` | ✅ 增量 ESP 配置，CMake 用 `LV_CONF_PATH` + `LV_KCONFIG_IGNORE` 应用 |
| `EspPlatform` | ✅ 适配器骨架，已用 stub 头做语法检查；真实 st77916/cst816 适配器待接 |
| RTTI / 异常 | ✅ 通过 `sdkconfig.defaults` 开启；未做去 RTTI 重构 |
| 桌面回归 | ✅ 默认 174/174、FreeType ON 174/174、rotation/RTL OFF 169/169、ASAN 174/174 |

## 3. ESP-IDF 日志后端（已实现）

- `framework/core/log/log.h`：`#ifdef ESP_PLATFORM` 时 include `log_esp.h`，否则保持原实现。
- `framework/core/log/log_esp.h`：
  - `LOG_DEBUG()/INFO/WARN/ERROR` 返回一个 `EspLogStream`，`operator<<` 写入固定 256 字节行缓冲，析构时 `esp_log_write(level, "lvglpp", "%s", line)`。
  - 支持 `const char*`、`std::string`、`std::string_view`、字符、整数、浮点、枚举、指针；不引入 `<filesystem>/<fstream>/<iostream>`。
  - 定义 `LVGLPP_LOG_DISABLE` 时宏展开为 `NullLogStream`，所有日志编译成空操作。
  - `Logger::Init/Configure` 保留为模板空实现，兼容旧调用（ESP 上没有文件日志）。
- `framework/core/log/log.cpp`：ESP 下为空翻译单元；IDF component 可以不编译它。

推荐配置：

```text
CONFIG_LOG_DEFAULT_LEVEL_INFO=y      # 或 Warning，降低串口开销
# 组件内可选
LVGLPP_LOG_DISABLE=1                 # release 构建彻底移除日志
```

注意：`LOG_*` 是流式调用，临时对象在整条语句结束时析构；不要在日志语句里做有副作用的操作。

---

## 4. 字体按需裁剪方案

### 4.1 现状与问题

- `framework/core/tools/fonts/*.c` 共 24 个文件，源文件合计约 90 MB；每个 face 1.7–9.2 MB 源文本。
- 生成参数：6 个字号（20/22/24/28/32/36）× Normal/Bold × 5 种语言字符集 × 8bpp × `--no-compress`。
- 即使在桌面，这也是明显的 flash 浪费；ESP32 上不可接受。

### 4.2 方案 A：构建期位图 profile（推荐，已实现生成器）

核心思想：**按产品实际需要的语言/字号/字重生成字体，而不是把整个 workbook 的所有语言都编进去。**

已新增 `framework/tools/i18n/gen_fonts_profile.py`（独立于桌面 `gen_fonts.py`，不会修改 `framework/core/tools/fonts/`）：

```bash
python3 -I framework/tools/i18n/gen_fonts_profile.py \
  --xlsx demo/smart_home/i18n/strings.xlsx \
  --langs zh-CN,en-US \
  --sizes 16,24 \
  --styles normal \
  --bpp 4 \
  --compress \
  --extra-charset ports/esp32/dynamic_charset.txt \
  --out ports/esp32/fonts \
  --emit-table
```

实现细节：

1. **按语言取字符集**：只取 `row.values[tag]`，不合并 5 种语言。
2. **固定范围**：ASCII、Latin-1、常用标点始终包含；CJK/假名/西里尔/阿拉伯形式按语言追加。
3. **bpp 与压缩**：默认 `--bpp 4`，`--compress` 开启 RLE；小字号可试 `--bpp 2`。
4. **字重**：默认只生成 Normal；标题需要 Bold 时用 `--styles normal,bold`。
5. **动态字符串**：`--extra-charset` 追加设备名/SSID/用户输入字符；无法枚举时用方案 B。
6. **生成字体表**：`--emit-table` 输出 `font_table_<lang>.h/.c`，包含 `LvglppFontFace {size, normal, bold}` 表。
7. **LvFontManager**：`LvFontManager::setProfile(faces, count)` 已实现；应用在启动/语言切换时把生成表注册进去，未注册时 ESP 回退 `LV_FONT_DEFAULT`。
8. **CI 校验**：`FontCoverageTests` 增加 ESP profile 覆盖测试；manifest 记录每个 face 的哈希。

已实测（16px、4bpp、compress）：`en-US` 约 97 KB 源文件；`zh-CN` 约 343 KB 源文件（CJK 字符集）。编译后的 flash 占用明显小于源文件，实际数值以 `idf.py size-components` 为准。`--emit-table` 会输出可用的 `font_table_<lang>.h/.c`。

预算参考（务必用 `idf.py size-components` 实测）：

| Profile | 估算 |
|---|---|
| 1 语言 × 2 字号 × Normal × 4bpp × 压缩 | 每个 face 约 100–250 KB（CJK 字符集 1000–2500 字） |
| 2 语言 × 2 字号 × Normal | 约 0.4–1 MB |
| 当前全量（5 语言 × 6 字号 × 2 字重 × 8bpp） | 十几 MB 量级，不可用 |

### 4.3 方案 B：运行期 Tiny TTF（有 PSRAM 时可选）

如果 UI 需要任意字号，或动态文本不可枚举：

1. 用 `fonttools pyftsubset` 生成每种语言的小 TTF：
   ```bash
   pyftsubset NotoSansSC-Regular.ttf \
     --text-file=charset_zh.txt \
     --output-file=NotoSansSC-zh-subset.ttf \
     --layout-features='*' --glyph-names --symbol-cmap --legacy-cmap
   ```
2. LVGL 开启 `LV_USE_TINY_TTF=1`，配置 glyph cache（`LV_TINY_TTF_CACHE_GLYPHS` 等）。
3. 把 subset TTF 放进 embedded binary 或 SPIFFS；`LvFontManager` 用 `lv_tiny_ttf_create_data()`/`lv_tiny_ttf_create_file()` 加载当前语言。
4. 优点：任意字号、动态字符；缺点：首次栅格化 CPU、RAM cache、PSRAM 依赖。

推荐产品策略：**A 为主，B 仅用于“动态数据/未知字符”兜底**；不要为了省事直接上全量 Noto TTF。

### 4.4 字体与 i18n 的联动

- `strings.xlsx` 仍是唯一字符来源；`gen_lv_i18n.py` 生成翻译，`gen_fonts.py` 生成对应语言的字体。
- 语言切换时同时调用 `LvTranslator::load()` 和 `LvFontManager::setLanguage()`；然后 `UiConfig::requestRebuild()` 重建页面。
- 只打包当前产品需要的语言；`--langs` 是构建参数，不是运行时开关。

---

## 5. EspPlatform 设计

### 5.1 解耦原则

SDK 不直接依赖 `st77916_driver`/`cst816_touch` 的头文件。由应用侧实现两个适配器：

```cpp
// ports/esp32/include/lvglpp/esp/EspHardware.h
namespace lvglpp::esp {

class DisplayAdapter {
public:
    virtual ~DisplayAdapter() = default;
    virtual bool init() = 0;
    virtual void shutdown() = 0;
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual lv_color_format_t colorFormat() const { return LV_COLOR_FORMAT_RGB565; }
    /// 启动一次 flush；异步 DMA 完成时调用 lv_display_flush_ready(display)。
    virtual bool flush(lv_display_t *display, const lv_area_t *area, uint8_t *pxMap) = 0;
    /// 硬件 MADCTL 旋转；返回 false 表示由 LVGL 软件旋转。
    virtual bool setRotation(uint16_t degrees) { (void)degrees; return false; }
};

class TouchAdapter {
public:
    virtual ~TouchAdapter() = default;
    virtual bool init() = 0;
    virtual void shutdown() = 0;
    /// 非阻塞：有坐标时填 data->point 并设置 state；无新样本时保持上一次状态。
    virtual void read(lv_indev_data_t *data) = 0;
};

} // namespace lvglpp::esp
```

应用侧适配器示例（伪代码，按你的驱动 API 填）：

```cpp
class St77916Display final : public lvglpp::esp::DisplayAdapter {
    bool init() override { return st77916_init(&panel_); }
    void shutdown() override { st77916_deinit(&panel_); }
    int width() const override { return 360; }
    int height() const override { return 360; }
    bool flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px) override {
        return st77916_flush_async(area, px, [disp] { lv_display_flush_ready(disp); });
    }
};

class Cst816Touch final : public lvglpp::esp::TouchAdapter {
    bool init() override { return cst816_init(&touch_); }
    void shutdown() override { cst816_deinit(&touch_); }
    void read(lv_indev_data_t *data) override {
        cst816_point_t p{};
        if (cst816_read(&touch_, &p)) {
            data->point.x = p.x;
            data->point.y = p.y;
            data->state = p.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        }
    }
};
```

### 5.2 EspPlatform 职责

```cpp
class EspPlatform final : public sys::Platform {
    const char *name() const noexcept override { return "ESP32"; }
    bool init() override;          // lv_tick_set_cb + display + indev
    void shutdown() override;      // 停止硬件；不删除 LVGL 对象
    lv_display_t *display() const override;
    void setTheme(lv_theme_t *) override {}
    void simulatePointerClick(std::int32_t, std::int32_t) override {}
    bool providesTick() const noexcept override { return true; }
    bool supportsPanelRotation() const noexcept override;
    bool applyOrientation(Orientation) override;
};
```

`init()` 顺序：

1. `displayAdapter_->init()` / `touchAdapter_->init()`。
2. `lv_tick_set_cb([]{ return static_cast<uint32_t>(esp_timer_get_time() / 1000); });`
3. `lv_display_create(w, h)`；`lv_display_set_color_format(RGB565)`。
4. 分配两块 draw buffer：`heap_caps_malloc(..., MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)`；PSRAM 只用于图像缓存，不建议做 DMA buffer。
5. `lv_display_set_buffers(disp, buf1, buf2, bytes, LV_DISPLAY_RENDER_MODE_PARTIAL)`。
6. `lv_display_set_flush_cb()`：调用 `displayAdapter_->flush()`；异步完成回调里 `lv_display_flush_ready()`。
7. 创建 pointer indev：`lv_indev_create()` + `lv_indev_set_read_cb()` + `lv_indev_set_display()`。
8. `lv_display_set_user_data()` / `lv_indev_set_user_data()` 存 `this`，供静态回调取回。

`applyOrientation()`：

- 优先 `displayAdapter_->setRotation(degrees)`（MADCTL 硬件旋转，无 CPU 成本）。
- 不支持时 `lv_display_set_rotation(display_, LV_DISPLAY_ROTATION_90/180/270)`（软件旋转，CPU 成本高）。

`shutdown()`：只停硬件，不 `lv_display_delete()`；Application 随后 `lv_deinit()` 释放 LVGL 对象。draw buffer 若由 Platform 分配，在此释放是可以的（LVGL 不再使用），但要在 `platform_->shutdown()` 后不再刷新；建议保留到 `lv_deinit()` 之后由静态生命周期管理，避免悬空。

### 5.3 启动方式

推荐两种：

```cpp
// A. Widget-only：现有 main.cpp 自己驱动
lv_init();
// display/touch 初始化同上
lv_timer_handler();  // 每 5 ms 或由 FreeRTOS task 驱动

// B. Full Application（Activity/StackView/PageHost）
auto app = std::make_unique<lvglpp::sys::Application>(
    std::make_unique<lvglpp::esp::EspPlatform>(display, touch));
app->initialize();
app->exec();
```

Full Application 路径需要 §2 的 guards；Widget-only 路径可以只编译 core/base/widgets/style/tools 的便携子集。

### 5.4 堆与 PSRAM（实测）

lvglpp 的 Widget 包装、`std::string`、`std::function`、信号槽和全局注册表会产生
大量小于 1 KB 的分配。ESP-IDF 默认 `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=16384`
会把这些分配全部留在内部 RAM；在 360×360 的小屏设备上，仅 UI 初始化就可能
吃掉约 52 KB 内部堆，导致后续语音/网络初始化 `ESP_ERR_NO_MEM`，甚至在
`time()` 首次加锁时 abort。

建议：

- 启用 `CONFIG_SPIRAM_USE_MALLOC`，并优先把 `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL`
  设为 `0`（或经过实测的小值）；`1024` 对 lvglpp 的小对象通常仍不够。
- draw buffer 用 `heap_caps_malloc(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)`，
  或静态 `.bss`；不要因为阈值调整把它们放进 PSRAM。
- 在 UI 初始化前后打印 `heap_caps_get_free_size(MALLOC_CAP_INTERNAL)`，
  确认留给音频/语音/协议栈的余量。smart-speaker 的完整记录见其
  `docs/lvglpp-ui-migration.md`。

### 5.5 触摸坐标与点击（CST816S 实测）

这块 360×360 面板的 CST816S 原始坐标已经是 0..359，**不要**再按常见的
240×240 面板乘 `360/240`。实测点底部 PLAY（约 180,335）时 `raw=(202,359)`，
被缩放成 `(303,359)`，点击落到按钮外，表现为“能滑动但点不中”。

- `cst816_read()` 直接返回 raw 坐标（必要时只 clamp 到 359）。
- `indevScrollLimit` 用 40px：触摸坐标抖动会被放大，20px 容易把 click
  判成 drag，从而吞掉 `LV_EVENT_CLICKED`。
- I2C 读失败不要当成 release；让 adapter 保持上一触摸状态，否则一次总线
  错误就会取消点击。
- 调试时临时打印 `CST816: raw=(x,y)`；如果 raw 已经到 359，说明无需缩放。

---

## 6. ESP-IDF 组件与分阶段计划

### 6.1 组件布局建议

```text
ports/esp32/
├── CMakeLists.txt          # idf_component_register(...)
├── sdkconfig.defaults      # RTTI/exceptions/PSRAM/FreeRTOS
├── lv_conf_esp32.h         # 或使用 CONFIG_LV_* Kconfig
├── EspPlatform.h/.cpp
└── include/lvglpp/esp/EspHardware.h
```

`CMakeLists.txt` 的职责：

- `EXTRA_COMPONENT_DIRS` 加入 `framework/lvgl`（LVGL 9.5 自带 `env_support/cmake/esp.cmake`）。
- 编译 `framework/core/{base,widgets,style,sys,tools}` 的便携子集。
- 排除：`sys/platform/{Sdl,Framebuf,Drm}Platform.cpp`、`EvdevInput.cpp`、`sys/automation/*`、`log/log.cpp`。
- `REQUIRES lvgl esp_timer esp_lcd driver`。
- `target_compile_definitions(... PUBLIC ESP_PLATFORM=1 LVGLPP_PLATFORM_ESP32=1)`。
- 字体只加入 profile 生成的文件，不要 glob 全部 `fonts/*.c`。

### 6.2 分阶段（建议顺序）

**Phase 0：可移植性层**

- 落地 `log_esp.h`（已完成）。
- 决定 RTTI/异常策略（A 或 B）。
- 加 `#ifdef ESP_PLATFORM`/`#ifndef ESP_PLATFORM` guards：Application、PlatformFactory、AssetManager、ImageManagerImpl、AssetPack、ThorVG include。**（已完成）**
- 准备 ESP 版 `lv_conf` 和 `sdkconfig.defaults`。**（sdkconfig.defaults 已提供；lv_conf 仍需按面板/PSRAM 固化）**
- 验收：IDF 下能编译一个只包含 log + Widget 的最小 TU。**（核心 TU 已用 stub 头做了 `-fsyntax-only` 检查）**

**Phase 1：Widget-only bring-up**

- EspPlatform 的 display/touch/tick 适配器。
- 一个 `Label + Image + PushButton` 页面，验证 flush、触摸、tick、PSRAM。
- 验收：触摸可点击、动画不撕裂、`idf.py size-components` 记录 RAM/flash 基线。

**Phase 2：Full Application + 资源**

- 接入 `Application`，embedded `common.pak` + `mountMemoryView`。
- `ImageManager` 同步加载（裁掉 prefetch/loadFile）。
- 验收：`Toast`/`Dialog`/`Image`/`Label` 正常；`Application::shutdown()` 在设备上无泄漏/崩溃。

**Phase 3：Activity/StackView/InputManager/PageHost**

- 接入 `StackView`/`BaseActivity`/`PageHost`。
- 触摸按键/手势映射到 `KeyEvent`（`InputManager::postKey()` 可在任意任务调用）。
- 验收：页面 push/pop、返回键、Dialog 模态、PageHost tab 状态恢复。

**Phase 4：字体 profile 与 i18n**

- 按 §4 生成 ESP profile；`LvFontManager::setLanguage()`。
- 验收：目标语言无缺字、字体 flash 在预算内、语言切换可重建页面。

**Phase 5：`ui_menu.cpp` 迁移**

- 逐屏替换为 `Widget/WidgetStore`：
  - `lv_obj_create` → `Widget`/具体控件。
  - `lv_obj_add_event_cb` → `onClicked/onKey/addLvglEventHandler`。
  - 长生命周期的交互控件放入成员或 `WidgetStore`。
  - 静态布局优先用 flex/grid + `Style/Theme`。
- 验收：功能对等、无 `lv_obj_t*` 长期持有、键盘/触摸两套输入都可用。

---

## 7. 需要你确认的输入

1. **芯片/内存**：ESP32-S3？PSRAM 容量？Flash 容量？是否启用 16MB flash？
2. **面板**：ST77916 分辨率、RGB565 字节序、SPI 频率、MADCTL 旋转能力、是否有 DMA 完成回调。
3. **触摸**：CST816 I2C 地址、INT/RST 引脚、是否支持多点；`read()` 是否阻塞。
4. **语言**：产品实际需要哪些语言？是否需要在运行时切换？动态数据（设备名/SSID）是否包含未知 CJK 字符？
5. **`ui_menu.cpp`**：目前是纯 `lv_*` 调用还是已有结构？多少屏/控件？
6. **功能取舍**：是否需要 GIF/QRCode/Lottie/视频？这些直接决定 flash 和 LVGL 配置。
7. **C++ 运行时**：能否接受 `CONFIG_COMPILER_CXX_RTTI=y` + `CONFIG_COMPILER_CXX_EXCEPTIONS=y`，还是必须 `-fno-rtti -fno-exceptions`？

拿到 1–3 和 5 后，可以按 Phase 0/1 直接落地 `ports/esp32` 的可编译版本。

---

## 8. 桌面能力回归清单

每次改动涉及 `ESP_PLATFORM` / `ports/esp32` 时，至少执行：

```bash
# 1) 默认桌面构建与单元测试
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build/target/bin && ./test_unit

# 2) 平台后端组合
cmake -S . -B build-sdl   -DLVGLPP_PLATFORM=SDL
cmake -S . -B build-fb    -DLVGLPP_PLATFORM=FRAMEBUF
cmake -S . -B build-drm   -DLVGLPP_PLATFORM=DRM

# 3) 可选能力组合
cmake -S . -B build-ft-on   -DLVGLPP_ENABLE_FREETYPE=ON
cmake -S . -B build-static  -DLVGLPP_ENABLE_ROTATION=OFF -DLVGLPP_ENABLE_RTL=OFF
cmake -S . -B build-lua     -DENABLE_LUA=ON
cmake -S . -B build-asan    -DCMAKE_BUILD_TYPE=Debug -DASAN=ON

# 4) 设计/i18n 一致性
cmake --build build --target design-check
cmake --build build --target i18n-check
```

验收原则：

- **不允许**出现“ESP 能编、桌面编不过”或“桌面功能被 guard 掉”的情况。
- `test_unit` 必须与改动前保持同样的测试数量/通过率（新增 ESP 专属测试除外）。
- FreeType、ThorVG、FFmpeg、evdev、filesystem 热更新、prefetch、automation、Lua、ws_gw 在桌面构建中仍然可用。
- 字体默认生成命令不变；ESP profile 输出到独立目录（如 `ports/esp32/fonts/`），不得覆盖 `framework/core/tools/fonts/`。
