# LVGL++ C++ SDK 使用指南

本指南面向使用 `sdk/` 中 `lvglpp` 库开发 LVGL 9.3 界面的工程师。SDK 的目标是：

- 保留 LVGL 的父子对象模型与灵活性；
- 让 C++ 包装对象的生命周期、事件回调和焦点管理可预测、可安全销毁；
- 提供统一的样式、主题、布局、按键与测试基础设施。

---

## 1. 构建与目录

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)

# 单元测试（headless）
./build/target/bin/test_unit
ctest --test-dir build/test --output-on-failure

# 带 AddressSanitizer
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DASAN=ON
cmake --build build-asan --target test_unit -j$(nproc)
./build-asan/target/bin/test_unit

# 四方向 + RTL（默认 ON）
cmake -S . -B build -DLVGLPP_ENABLE_ROTATION=ON -DLVGLPP_ENABLE_RTL=ON

# 静态产品：编译期移除旋转/RTL 代码，零额外运行时开销
cmake -S . -B build-static -DLVGLPP_ENABLE_ROTATION=OFF -DLVGLPP_ENABLE_RTL=OFF
```

应用必须从 `build/target/bin` 运行；资源按 `<cwd>/common.pak` → `<cwd>/<exe>.pak` 查找（目录模式为 `*.asset/`），可用 `LVGLPP_ASSET_DIR` / `LVGLPP_ASSET_PACK` 覆盖。

主要头文件：

| 头文件 | 内容 |
|---|---|
| `core/widgets/Widget.h` | 所有控件的基类 |
| `core/widgets/WidgetStore.h` | 长生命周期包装对象容器 |
| `core/base/Connection.h` | 事件连接 token |
| `core/widgets/KeyEvent.h` | 按键事件类型 |
| `core/sys/InputManager.h` | 焦点作用域、全局按键路由 |
| `core/sys/UiConfig.h` | 四方向、RTL、逻辑屏幕尺寸 |
| `core/style/Style.h` / `Theme.h` | 样式与主题 |
| `core/widgets/GridLayout.h` | Grid 描述符解析 |

---

## 2. 对象模型与所有权

### 2.1 基本规则

- **LVGL 父对象拥有子 `lv_obj_t`**。
- `Widget` 包装对象是子对象的**非拥有句柄**：子包装对象可以在任意时刻销毁，LVGL 对象仍由父级持有。
- 只有以下包装对象在析构时删除自己的 `lv_obj`：
  - 没有逻辑父对象的顶层对象（Activity root）；
  - creator 实际把对象挂到 `lv_layer_top()/lv_layer_sys()` 的层对象（`Dialog` 根、`ToastImpl` 等）；
  - 通过 `ownsLvglObject()` 可查询是否为拥有者。
- `Widget` 禁止拷贝与移动。

```cpp
// 合法且常见：局部包装对象只用于构建 UI
{
    auto title = std::make_unique<widgets::Label>("Hello", 0xFFFFFF, root);
    title->setAlignment(LV_ALIGN_TOP_LEFT, 16, 8);
} // title 析构；label 仍存在于 root 下
```

### 2.2 为什么短命包装对象是安全的

每个包装对象都会：

1. 在全局注册表中登记 `lv_obj -> wrapper`，供 `childAt()` / `findChild()` / `InputManager` 使用；
2. 给 `lv_obj` 注册 `LV_EVENT_DELETE` watchdog；
3. 析构时移除自己注册的全部 LVGL 事件回调，并从焦点组中移除。

因此：

- 包装对象析构后，后续点击/按键不会回调到已释放的 C++ 对象；
- 父树先被删除时，所有子包装对象会自动失效（`isAttached() == false`），后续方法调用是安全的 no-op；
- LVGL 长期持有的调用方指针（grid 描述数组、图片路径、样式、ImageButton 源）由注册表的 keepalive 保存，直到 `lv_obj` 被删除。

> 注意：包装对象析构后，它的事件处理器也会消失。**带回调的控件必须保持包装对象存活**，见 2.3。

### 2.3 WidgetStore：保持交互控件存活

```cpp
#include "core/widgets/WidgetStore.h"

widgets::WidgetStore store;

auto *ok = store.emplace<widgets::PushButton>(
    120, 40, widgets::PushButton::Blue, "OK", parent);
ok->onClicked([] { /* ... */ });
```

- `store` 随 Activity/页面一起销毁；其中的包装对象保持存活，回调持续有效。
- 纯展示、无回调的控件可以继续使用局部包装对象。
- 局部包装对象上设置回调是错误用法（回调会随包装对象销毁而失效）。

### 2.4 包装已存在的对象（adoption）

```cpp
auto *page = new widgets::Widget(
    widgets::Widget::AdoptExistingTag::Adopt,
    lv_tabview_add_tab(tabview, "Tab"), tabview, false);
```

用于包装 LVGL 内部创建的 `lv_obj`（TabView page、MsgBox footer button 等）。默认非拥有。

### 2.5 父对象查询

`getParent()` 返回**最近的存活包装对象祖先**（沿 LVGL 父链查找注册表），而不是缓存的原始指针；`setParent()` 会同步更新 LVGL 层级与所有权判定。

---

## 3. 事件与信号

### 3.1 Widget 事件

```cpp
auto conn = button->onClicked([] { /* ... */ });
auto scoped = core::ScopedConnection(button->onLongPressed([] { /* ... */ }));

conn.disconnect(); // 手动断开；可拷贝 token
```

- 返回的 `core::Connection` **析构时不会自动断开**，忽略返回值是安全的。
- 需要 RAII 时使用 `core::ScopedConnection`。
- 可用：`onClicked / onPressed / onReleased / onLongPressed / onKey / onFocus / onBlur / onDestroyed`。
- 旧接口 `setOnClickedListener` 等仍然可用。
- `onDestroyed` 在包装对象析构或 `lv_obj` 被删除时触发一次。

### 3.2 原生 LVGL 回调

```cpp
item->addLvglEventHandler(myCallback, LV_EVENT_VALUE_CHANGED, context);
item->addChildLvglEventHandler(childObj, cb, LV_EVENT_CLICKED, context);
```

回调会随包装对象析构自动移除。请勿再直接对由 `Widget` 管理的对象调用 `lv_obj_add_event_cb(..., this)`，否则可能在包装对象销毁后悬垂。

### 3.3 Object 信号

```cpp
auto conn = core::Object::connect(sender, "changed", receiver, [] { ... });
```

带 receiver 的连接会在 receiver 析构时自动断开；sender 析构后 `Connection` token 自动失效。`Object` 不可拷贝/移动。

---

## 4. 焦点与键盘/物理按键

### 4.1 自动作用域

- `BaseActivity::onResume()` 会为 Activity root 压入焦点作用域；
- `BaseActivity::onPause()/onDestroy()` 弹出；
- `Dialog::open()` 压入模态作用域，`close()` 弹出；ESC 默认关闭对话框；
- Activity 的返回导航由应用自行绑定（本 SDK 不默认 pop 页面）。

### 4.2 控件侧 API

```cpp
button->setFocusable(true);   // 交互控件构造时默认已开启
button->requestFocus();
bool focused = button->hasFocus();

button->onKey([](widgets::KeyEvent &e) {
    if (e.key == widgets::Key::Enter) {
        e.consume();          // 不再向父级/全局传播
    }
});
button->onFocus([](bool focused) { /* ... */ });
```

- 未消费的按键沿包装对象父链冒泡，最后交给全局监听器。
- focusable 控件自动获得 `LV_STATE_FOCUS_KEY` 焦点描边（来自 `Theme::focusStyle()`）。

### 4.3 全局按键与返回键

```cpp
auto &input = sys::InputManager::getInstance();

auto conn = input.addGlobalKeyHandler([](widgets::KeyEvent &e) {
    if (e.key == widgets::Key::Escape) {
        e.consume();
        // 自定义返回逻辑
    }
});

auto token = input.addBackHandler(sys::BackPriority::App, [] {
    // 应用级 ESC 回退；返回 true 表示已消费
    return false;
});
input.removeBackHandler(token);
```

作用域和全局处理器都返回 `bool`（true = 已消费）；优先级顺序为
`SystemDialog > Dialog > Popup > Page > Activity > App`，同优先级 LIFO：

```cpp
input.pushScope(root, [] { return true; }, sys::BackPriority::Activity);
input.pushModalScope(dialogRoot, [] { return true; }, sys::BackPriority::Dialog);
```

### 4.4 物理按键 / 后台线程

```cpp
// 任意线程安全
input.postKey(widgets::Key::Up, true);
input.postKey(widgets::Key::Up, false);

// 字符键
input.postKeyRaw('A', true);
input.postKeyRaw('A', false);
```

- 按键队列由虚拟 KEYPAD indev 在 UI 线程消费。
- 硬件侧（GPIO、串口、自定义驱动）只需调用 `postKey()/postKeyRaw()`。
- 按住不放会产生 `Repeat`，超过 500ms 产生 `LongPress`。
- SDL 键盘仍走原生 SDL keyboard indev；由于 LVGL 只对 Enter 提供完整 release 语义，非 Enter 键的 `Release` 在下一个按键或焦点变化时合成。需要真实 release 语义的硬件请使用 `postKey`。

### 4.5 事件顺序

1. 焦点控件自身 `onKey`；
2. 父级包装对象 `onKey`（冒泡）；
3. `InputManager` 全局监听器；
4. 未消费的 ESC → 当前作用域返回处理器 / 全局返回处理器；
5. LVGL 原生焦点导航（NEXT/PREV/ENTER）继续工作。

---

## 5. 样式与主题

### 5.1 Style

```cpp
#include "core/style/Style.h"

core::Style card;
card.bgColor(0x252B3E).radius(16).border(2, 0x485479).padAll(12);

item->applyStyle(card);
```

- `Style` 是共享句柄，拷贝共享同一个 `lv_style_t`。
- **首次 apply 后不可变**；继续调用 setter 会记录错误并 no-op。派生变体请用 `clone()`。
- `Widget::applyStyle()` 会把 Style 保存在对象 keepalive 中，因此栈上 Style 离开作用域也安全。
- `removeStyle(style, selector)` 移除并释放对应 keepalive。

### 5.2 Theme

```cpp
core::Theme &theme = core::Theme::instance();
theme.setMode(core::ThemeMode::Dark);   // 或 Light

uint32_t surface = theme.hex(core::ColorRole::Surface);
lv_color_t ring  = theme.color(core::ColorRole::FocusRing);

item->applyStyle(theme.focusStyle(), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
```

- 语义色角色见 `core/style/Theme.h` 的 `ColorRole`。
- 深色/浅色 palette 都来自设计令牌，不再手写：`design/tokens.json` 是唯一来源，
  `sdk/tools/design/sync_tokens.py generate` 生成 `sdk/config/color_sheet.h`
  （深色/基础值）与 `color_sheet_light.h`（浅色值）。
- 设计令牌可从 Figma 同步（Tokens Studio 导出或 Figma Variables REST），
  详见 `design/README.md`。生成头文件已提交，普通构建不依赖 Python 或网络。
- **主题切换不回溯已创建控件**；运行期切换主题后需重建 UI（smart_home 设置页
  已通过 `UiConfig::requestRebuild()` 接入）。
- `PushButton` 的配色由 `Theme::buttonPalette()` 驱动，仍保留 `ColorStyle` 枚举。

### 5.3 设计令牌

```bash
# 修改 design/tokens.json 后重新生成
python3 -I sdk/tools/design/sync_tokens.py generate

# CI / 本地检查生成头文件是否过期
cmake --build build --target design-check
```

- `form::design::CLR_*` 是 `ColorToken`，读取时会按当前 `ThemeMode` 隐式转换为
  深色或浅色值；已有调用点无需改写即可跟随 `Theme::setMode()`。
- 需要显式取值时用 `CLR_X.darkValue()` / `CLR_X.lightValue()`，或
  `form::design::light::CLR_X`。它们是运行期值，不是 `constexpr`。
- 控件在创建时保存颜色；切换主题后需要重建 UI。smart_home 设置页会调用
  `sys::UiConfig::requestRebuild()`，由主循环安全地重建当前页面。
- 需要语义角色（而不是具体 token）时使用 `core::Theme::color(ColorRole)`。
- 不要直接编辑生成的两个头文件；新增角色时同步更新 `Theme.cpp` 的 `kPalette`
  （`static_assert` 会在漏掉角色时编译失败）。

### 5.4 状态

```cpp
item->setState(LV_STATE_CHECKED, true);
item->addState(LV_STATE_PRESSED);
item->clearState(LV_STATE_DISABLED);
```

---

## 6. 布局

### 6.1 Flex

```cpp
container->setFlexFlow(LV_FLEX_FLOW_ROW_WRAP)
         .setFlexAlign(LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
child->setFlexGrow(1);
```

### 6.2 Grid

```cpp
container->setGridColumns("1fr 2fr auto 100px");
container->setGridRows("50px 1fr");
child->setGridCell(1, 0);                 // column 1, row 0
child->setGridCell(0, 1, 2, 1);           // 跨 2 列
child->setGridCellColumnAlign(LV_GRID_ALIGN_CENTER);
```

支持的描述符：`Nfr`、`auto`、`content`、`Npx` 或裸整数像素。最多 32 轨。非法描述符整串拒绝并保持上一份模板。两轴始终有合法描述符，缺失轴默认 `1fr`。

### 6.3 尺寸

- `getWidth()/getHeight()` 返回**实时布局尺寸**；首次布局前为 0 时回退到 `setSize()` 记录的请求尺寸。
- `setSizePercent(w, h)` 支持百分比尺寸。
- 提供 `setPadding* / setMargin*` 全套便捷方法。

### 6.4 横竖屏与 RTL

```cpp
sys::UiConfig &cfg = sys::UiConfig::instance();
cfg.requestOrientation(sys::Orientation::Portrait);        // 0/90/180/270
cfg.requestLayoutDirection(sys::LayoutDirection::Rtl);     // Arabic
cfg.requestRebuild();                                      // re-translate current page
cfg.feedGravity(gx, gy, gz);                               // sensor thread, filtered

int w = cfg.width();
int h = cfg.height();
bool portrait = cfg.isPortrait();
```

- 所有 `request*` 都是线程安全的原子请求；`Application::exec()` 在 `lv_timer_handler()` 前调用 `UiConfig::processPending()`，所以不要在 LVGL 回调里同步重建 UI。
- `StackView` 把配置变更分发给每个 `Activity::onConfigurationChanged`；`BaseActivity` 调整 root 尺寸，具体页面通过 `PageHost::saveState()`/`restoreState()` 保留每 Tab 的页面栈与 args 后重建；滚动位置和打开的对话框不保留。
- `Widget::setAlignment/setAlignmentTo/setPos` 会按 RTL 镜像显式 LEFT/RIGHT/OUT_* 和 x 偏移。**flex/grid 的 RTL 反向由 LVGL 自动完成，不要手动镜像**。`setMirrorInRtl(false)` 可关闭某个控件的镜像；`Chart` 固定 LTR。
- 方向图片使用 `Image::setRtlAsset(path)`；LVGL 9.3 不支持负 scale 镜像。
- 启动/调试环境变量：`LVGLPP_ORIENTATION=landscape|portrait|reverse-landscape|reverse-portrait`、`LVGLPP_RTL=1`、`LVGLPP_PANEL_ROTATION_OFFSET=90`、`LVGLPP_SDL_FIXED_PANEL=1`、`LVGLPP_DISABLE_PANEL_ROTATION=1`。smart_home 还支持 `1/2/3/4` 切换四方向、`R` 切换 RTL。
- 静态产品零开销：`-DLVGLPP_ENABLE_ROTATION=OFF -DLVGLPP_ENABLE_RTL=OFF`，旋转/RTL 代码、每控件镜像状态和矩阵变换都会在编译期移除；`processPending()` 仍保留（每帧一次原子读取），用于语言切换时的页面重建。
- 平台：FBDEV 用 PARTIAL 软件旋转；SDL 默认通过窗口 reshape 模拟横竖屏（fixed-panel 仅 0/180）；DRM 用 DIRECT + matrix 软件旋转，CPU 成本需实测。`LV_DRAW_TRANSFORM_USE_MATRIX` 由 rotation 开关自动定义。

---

## 7. 控件

原有控件：`Label`、`PushButton`、`Image`、`GifImage`、`LottieCanvas`、`QrCode`、`Video`、`Arc`、`ProgressBar`、`Slider`、`SpinBox`、`CheckBox`、`Switch`、`Dropdown`、`Roller`、`TextArea`、`Keyboard`、`Led`、`Line`、`Chart`、`Table`、`Calendar`、`TabView`、`ListView`、`PageView`、`PageIndicator`、`Loader`（配合 `LoaderComponent`）、`TouchArea`、`Dialog`、`Toast` 等。

所有控件都继承 `widgets::Widget`（原 `BaseItem`）。常用现代命名：

```cpp
widget->lvObj();                 // 底层 lv_obj_t*（原 getLvglItem）
widget->setEnabled(false);       // 只切换 LV_STATE_DISABLED，不再隐式改 CLICKABLE
widget->isEnabled();             // 原 getEnable
widget->isVisible();             // 原 getVisible
widget->setAlignment(...);       // 原拼写 setAligment
```

旧名称仍以 `[[deprecated]]` inline 转发保留在头文件中，新代码请使用现代名称。`Image` 是例外：`setEnabled(true)` 会把它变成可点击目标（返回箭头等既有用法因此保持有效）。

`Label::setMaxLength(n)` 按“显示字符预算”工作：`n > 3` 时中间插入 `...`，`n <= 3` 时只保留前 n 个字符；`Label::setLongMode(maxWidth, AutoEllipsis|MiddleEllipsis)` 按像素宽度逐行省略，二者都按 UTF-8 字符簇切分，不会拆开中文或组合字符。`setLongMode(lv_label_long_mode_t)` 直接设置 LVGL 原生 long mode。启用 recolor 标记时自动省略会关闭，避免截断 `#RRGGBB` 标记。

新增控件：

| 控件 | 说明 |
|---|---|
| `Spinner` | 旋转加载指示器 |
| `ImageButton` | 三图拼合按钮，支持 Released/Pressed/Disabled/Checked 状态 |
| `MsgBox` | 标题/正文/按钮消息框，按钮自动加入焦点组 |
| `Scale` | 刻度尺，支持范围、刻度数、角度、模式 |

交互控件（PushButton、Slider、Switch、Dropdown、Roller、CheckBox、TextArea、Arc、Calendar、Table、ImageButton、Scale 等）默认 focusable。

### 7.1 资源管理

资源以**逻辑路径**寻址（相对资产根、`/` 分隔），例如 `image/40x40/icon_warn.png`、`fonts/NotoSansSC-Regular.ttf`、`lottie/camera.json`。

- **资产树**：`assets/common/` 存放所有 app 共享的字体、图片与 Lottie；`assets/apps/<target>/` 存放 app 私有资源。构建时生成 `common.pak` 与 `<target>.pak`。
- **`respack`**：无外部依赖的打包工具。

```bash
respack pack <asset-dir> <out.pak> [--exclude <glob>]...
respack list <in.pak>
respack verify <in.pak>
```

格式 `LVPAK1`：40 字节 header + 8 字节对齐的原始文件数据 + 按路径排序的索引；索引与每个条目都有 CRC-32 校验；路径必须是规范化的相对路径（拒绝 `..`、绝对路径、反斜杠与重复路径）。输出是确定性的，可复现。

- **`tools::AssetManager`**：只读多源挂载，后挂载的源优先级更高（`common.pak` → `<exe>.pak`）。

```cpp
auto &assets = tools::AssetManager::getInstance();
assets.mountPack("build/target/bin/common.pak");   // Application::initialize() 自动完成

tools::AssetHandle asset = assets.open("image/40x40/icon_warn.png");
const std::uint8_t *bytes = asset->data();
std::size_t         size  = asset->size();
```

  - Pack 使用只读 `mmap`（失败时回退为整文件读入），`AssetHandle` 是零拷贝视图；`reset()` 后已发出的 handle 仍然有效。
  - Directory 源每次 `open` 重新读取文件，支持热更新；符号链接逃逸会被拒绝。
  - `Application::initialize()` 自动挂载 `<cwd>/common.pak` 与 `<cwd>/<exe>.pak`（或对应 `.asset/` 目录）；`LVGLPP_ASSET_DIR`、`LVGLPP_ASSET_PACK` 可覆盖。
  - `A:` 只读 LVGL FS 驱动：`lv_fs_open("A:/image/...")`，供 GIF / FreeType 等需要文件路径的组件使用。
  - `AssetManager::materialize(path, out)` 返回真实文件路径；pack 条目会解压到进程临时目录，供绕过 LVGL FS 的库使用（FreeType、ffmpeg）。
  - 缺失资源每个路径只告警一次。

- **`tools::ImageManager`**：同步解码 + 安全 LRU + 可选后台预取。

```cpp
// 默认 32 项 / 64 MB；0 表示不保留缓存引用。
tools::ImageManager::setCacheLimits(16, 32ull * 1024 * 1024);

tools::ImageHandle handle = tools::ImageManager::load("image/20x20/icon_model20.png");
lv_image_set_src(obj, handle->descriptor());
// handle 必须至少活到 lv_obj 不再渲染该图；Image 控件会自动
// 把它放进 Widget 注册表 keepalive。

tools::ImageManager::prefetch(std::vector<std::string>{"image/a.png", "image/b.png"});
tools::ImageManager::waitForPrefetch();   // 测试/退出时等待队列排空

tools::ImageManager::clearCache();        // 只释放缓存引用；正在使用的图仍存活
```

  - 解码使用 `stb_image`（PNG/JPEG 等）→ LVGL ARGB8888（内存序 BGRA）；尺寸上限 16384×16384、解码缓冲上限 256 MB。
  - 缓存淘汰只丢弃 LRU 强引用；控件持有的 `ImageHandle` 保证像素存活。
  - `prefetch` 在单后台 worker 上读取资产并解码，**绝不调用 LVGL**；所有 stb 调用通过全局 mutex 串行化。
  - `loadFile(path)` 读取真实文件；`invalidate()` / `Image::reload()` 用于目录模式热更新。
  - `shutdown()` 停止 worker 并清空缓存（测试与退出路径会调用）。
- **控件数据源**：
  - `Image("image/x.png")` 使用资产；`Image(FileSource{"path.png"})` / `setFile()` 使用真实文件。
  - `ImageButton::setSource()` 接受三个资产路径（内部解码为 descriptor 并 keepalive）；`setSourceFile()` 使用真实文件。
  - `LottieCanvas::setAsset()` 读取 pack 字节并调用 `lv_lottie_set_src_data()`；`setFile()` 走 ThorVG 的文件接口。
  - `GifImage::setAsset()` 转换为 `A:/...`；`setFsPath()` 接受显式 LVGL 路径。
  - 内联 base64 图片使用 `Image(Base64Source{data, width, height}, parent)` 或 `Image::setBase64()`，不经过资产管线。
- **字体**：默认使用 `sdk/core/tools/fonts/` 下的点阵字体（静态数据），按 Excel 文案（`smart_home/i18n/strings.xlsx`）逐字号裁剪生成，`NotoSC_<size>_{Normal,Bold}` 覆盖中/英/日/俄/阿，Bold 通过 `NotoArabic_<size>_Bold` fallback 补齐阿拉伯语。启用 FreeType（`-DLVGLPP_ENABLE_FREETYPE=ON`）并通过 `LvFontManager::enableFreetype(true)` 切换后，才通过 `AssetManager::materialize()` 获取 TTF 路径；关闭时 `assets/common/fonts/*.ttf` 不进入 `common.pak`。生成/校验脚本见 `sdk/tools/i18n/`。
- **构建**：`LVGLPP_ASSET_MODE=pack|directory|both`（默认 `pack`）；`lvglpp_attach_assets(<target>)` 负责 common 与 app 资源的打包/暂存。

---

## 8. Timer 与 Animation

```cpp
// 单次 timer：不会自删除，owner 保持控制权
auto timer = sys::Timer::create([](sys::Timer *) { /* ... */ }, 1000,
                                sys::Timer::Mode::OneShot);

// 周期 timer
auto ticker = sys::Timer::create([](sys::Timer *) { /* ... */ }, 100);
ticker->pause();
ticker->resume();
ticker->restart();
```

```cpp
sys::Animation anim;
anim.setTarget(&widget)
    .setDuration(300)
    .setValues(0, 100)
    .setRepeatCount(LV_ANIM_REPEAT_INFINITE)
    .setExecCallback([](widgets::Widget *target, int32_t value) {
        target->setOpacity(value / 100.0);
    })
    .start();
```

- `Animation` 可在 start 后移动；目标控件销毁时自动停止。
- `Timer` 回调中销毁 timer 是安全的。

---

## 9. 测试

- `test/TestMain.cpp` 创建 headless 800x480 display，并注册一个立即完成 flush 的回调。
- 测试框架：`TEST(name) { CHECK...; return true; }`。
- 建议为新增控件/行为补充：
  - 生命周期（包装对象先亡、父树先亡、adoption）；
  - 事件连接与断开；
  - 焦点/按键；
  - 样式应用到布局属性；
  - Timer/Animation 销毁顺序。

---

## 10. 迁移注意事项

1. `Widget`/`Object` 已禁止拷贝与移动。
2. `getParent()` 返回最近存活包装对象祖先，而不是构造时缓存的指针。
3. `getWidth()/getHeight()` 返回实时尺寸（带请求尺寸回退）。
4. 包装对象析构后其事件回调失效；带回调的控件请放入成员或 `WidgetStore`。
5. `childAt()` 只返回仍有存活包装对象的子节点；未包装的内部节点返回 `nullptr`。
6. `Delegate` 仍保留“清除时删除自身 lv_obj”的行为，用于 ListView/PageView 行回收。
7. `Style` 挂载后不可变；主题切换不回溯已有控件。

---

## 11. 页面管理

### 11.1 Activity / StackView

```cpp
auto page = std::make_shared<MyActivity>();
sys::StackView::getInstance().pushView(page, arg);
sys::StackView::getInstance().popView(result);

// activity result：新页面 pop 时把结果交给调用方
sys::StackView::getInstance().pushView(page, arg, [](std::any result) {
    // 处理子页面返回值
});

// 启动模式（Android 风格）
stack.pushView(page, arg, {}, sys::StackView::LaunchMode::SingleTop);
stack.pushView(page, arg, {}, sys::StackView::LaunchMode::ClearTop);
stack.popTo(2);
stack.popToRoot();
```

`Activity` 可覆盖 `routeId()`（默认 `typeid`）、`onNewArguments()`（SingleTop/ClearTop 复用）、`onBackPressed()`（返回 true 表示自行处理）。生命周期：`onCreate(arg)` → `onResume()` → `onPause()` → `onDestroy()`。
- 在 `exec()` 之前多次 `pushView()` 是安全的：`StackView::start()` 会按栈顺序初始化所有已入栈页面，仅顶层处于 resumed 状态。
- 入场和退场动画期间再次 push/pop 会被排队，动画结束后依次执行，不会出现两次 pop 回调或栈下溢。
- 返回键统一走 `InputManager::dispatchBack()`：Dialog/Popup → PageHost → Activity（`onBackPressed` 或 `popView`）→ App 回退。
- 手动切换页面可见性（例如 smart_home 的 tab 容器）后，应调用 `BaseActivity::refreshFocus()`，让键盘焦点重新收集当前可见控件：
  ```cpp
  _pages[i]->setVisible(i == index);
  refreshFocus();
  ```
- `StackView::notifyAllUi()` 会把事件发送给栈中所有 Activity（包括 paused 的），需要只让可见页面响应时请在 `onNotifyUI` 中自行判断。
- `shutdown()` 会取消页面切换动画并依次调用 `onPause/onDestroy`。

### 11.2 PageHost（懒加载页面容器）

`widgets::PageHost` 用于“一个 Activity 内多页面切换”的场景（tab、抽屉页、向导页等）：

```cpp
#include "core/widgets/PageHost.h"

auto *host = new widgets::PageHost(root);       // 或放入成员 unique_ptr
host->setSize(800, 420);

host->addPage("Home", [](widgets::PageContext &ctx) {
    auto *title = ctx.emplace<widgets::Label>("Home", 0xFFFFFF, ctx.root());
    title->setAlignment(LV_ALIGN_TOP_LEFT, 16, 8);

    ctx.onShow([] { /* 恢复定时器/动画 */ })
       .onHide([] { /* 暂停定时器/动画 */ })
       .onDestroy([] { /* 释放外部资源 */ });
});

host->addPage("Settings", [](widgets::PageContext &ctx) {
    auto *slider = ctx.emplace<widgets::Slider>(ctx.root());
    slider->setSize(180, 20);
});

host->show(0);   // 首次 show 才构建页面
host->show(1);   // 构建并切换；Home 保留在内存中

// 非 root 子页面：压在当前 Tab 的历史栈上
const int detail = host->addPage("Detail", [](widgets::PageContext &ctx) {
    ctx.onArguments([](const std::any &args) { /* 读取进入参数 */ })
       .onResult([](std::any result) { /* 子页面 pop 回来 */ })
       .onPause([] { /* 被覆盖 */ })
       .onResume([] { /* 重新可见 */ });
}, true, false);

host->push(detail, std::any(42));
host->pop(result);          // 返回上一页
host->popToRoot();          // 回到当前 Tab 的根页
host->back();               // 用于返回键：栈深度 > 1 时 pop
```

特性：

- **懒加载**：`addPage(name, builder, lazy=true, root=true)` 默认不构建，首次 `show/push` 时调用 builder；root 页面是 Tab，非 root 页面用于 `push`；
- **每 Tab 独立历史**：`show(root)` 切换 Tab 并恢复该 Tab 自己的栈顶；再次 `show` 当前 Tab 会 `popToRoot()`；
- **参数/结果**：`push(index, args)` + `PageContext::onArguments/onResult`，参数用 `std::any`（需可拷贝，重对象用 `shared_ptr`）；
- **页面生命周期**：`onArguments/onNewArguments/onShow/onHide/onPause/onResume/onDestroy/onResult`；
- **返回键**：PageHost 注册 Page 优先级 back handler，栈深度 > 1 时先消费，否则交给 Activity/StackView；
- **缓存策略**：
  - `KeepAll`（默认）：访问过的页面保留，切换最快；
  - `ReleaseOnHide`：离开根 Tab 即销毁其 built 页面，entry/args 保留，下次重建；
  - `MaxCached(n)`：最多保留 n 个当前栈之外的页面，按最近显示时间淘汰；当前栈上的祖先永不淘汰；
- **自动焦点刷新**：切换后刷新包含 PageHost 的焦点 scope；隐藏 Tab 内的控件不会进入焦点组；
- **页面级 WidgetStore**：`ctx.emplace<T>()` 创建的交互控件随页面销毁而释放，不会泄漏回调；
- **配置变更恢复**：`saveState()` / `restoreState()` 保存每 Tab 的页面栈与 args；smart_home 在 `onConfigurationChanged` 中先保存、重建后恢复（页面内部滚动等瞬态不保留）；
- `release(i)` / `releaseAll()` 可手动释放页面；`currentIndex()/currentRoot()/depth()/history()/page(i)/isBuilt(i)` 用于状态查询。

smart_home 已迁移到 `PageHost`：启动时只构建 Home 页，其余四个 tab 首次点击时才构建，访问后默认保留状态。

### 11.3 TabView / PageView

- `TabView::addTab(name)` 返回 `std::unique_ptr<Widget>`：包装对象是页面内容的非拥有句柄，控件本身由 TabView 持有；`TabView::content(idx)` 返回对应页面 `lv_obj_t*`。
- `PageView` 的滚动索引有边界与宽度保护；挂起的异步页面创建会在包装对象销毁或 lv_obj 删除时取消。
- `ListView`/`PageView` 目前只增不回收 delegate；数据量很大时应实现窗口化复用，避免隐藏行持续占用内存。

### 11.4 Dialog / OverlayManager（层级与优先级）

`Dialog::open()/close()` 会自动向 `OverlayManager` 注册/注销。优先级（数值大者优先）：

```text
SystemDialog (400) > Dialog (300) > Popup (200) > Toast (10, 仅显示)
```

- `Dialog::Layer::System` 使用 `lv_layer_sys` 且默认 `SystemDialog` 优先级；`Layer::Application` 使用 `lv_layer_top` 且默认 `Dialog`。
- 多个对话框按 `(priority, 打开/置顶顺序)` 排 z-order；返回键从最高优先级开始，同优先级 LIFO。
- `Dialog::setPriority()` 影响下一次 `open()`；需要运行时置顶可调用 `OverlayManager::bringToFront()`。
- `OverlayManager::registerTransient()` 用于 Toast 等非焦点覆盖层：不创建 InputManager scope，不消费返回键，不阻挡键盘焦点。
- 自定义覆盖层可用 `attach(root, priority, onBack)` 接入同一套 z-order/返回链；`detach(token)` 幂等。
- 配置变更会关闭 Dialog（`LVGLPP_DYNAMIC_UI` 下），由 owner 按需重新打开。

### 11.5 Application、Platform 与退出

`sys::Application` 只负责通用生命周期（LVGL 初始化、资源挂载、事件队列、信号、主循环、清理）；显示/输入后端通过 `sys::Platform` 组合注入，后端类型不与 Application 继承体系耦合。

```cpp
#include "core/sys/Application.h"
#include "core/sys/StackView.h"

int main(int argc, char **argv)
{
    auto app = lvglpp::sys::createApplication(argc, argv);
    if (!app || !app->initialize())
    {
        return EXIT_FAILURE;
    }

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<MyActivity>());
    return app->exec();
}
```

- **平台选择**：`-DLVGLPP_PLATFORM=AUTO|SDL|FRAMEBUF|DRM`（默认 `AUTO`：非 ARM 选 SDL，ARM 选 FRAMEBUF）。CMake 只定义一个 `LVGLPP_PLATFORM_*` 宏，`sdk/lv_conf.h` 据此启用对应的 LVGL 驱动。
- **后端**：`SdlPlatform`、`FramebufPlatform`、`DrmPlatform`（`core/sys/platform/`）。新增后端实现 `Platform`（`name/init/shutdown/display/setTheme/simulatePointerClick/providesTick`）并在 `PlatformFactory.cpp` 注册即可。
- **生命周期**：
  1. `initialize()`：`lv_init` → 资产挂载 + `A:` 驱动 → 日志 → SIGINT/SIGTERM → ThorVG → `platform_->init()` → tick 兜底 → `InputManager::install()` → `onInit()`（子类钩子）。
  2. `exec()`：排空事件队列 → `lv_timer_handler()` → `min(idle, 5ms)` 睡眠；`requestQuit()` 或信号后退出并返回退出码。
  3. `shutdown()`：`onShutdown()` → 停止 tick → `StackView::shutdown()` → `ImageManager::shutdown()` → `InputManager::shutdown()` → `platform_->shutdown()` → `lv_deinit()` → 资源/信号恢复。幂等，部分初始化失败也安全。
- **退出来源**：
  - `requestQuit(code)` 可从任意线程调用，首个 code 生效；`exec()` 正常返回。
  - `SIGTERM`/`SIGINT`：退出码分别为 143/130。`SDL_HINT_NO_SIGNAL_HANDLERS` 禁止 SDL 接管信号。
  - SDL 窗口关闭：`SdlPlatform` 用 `SDL_SetEventFilter` 拦截 `SDL_QUIT`/`SDL_WINDOWEVENT_CLOSE`，调用 `requestQuit()` 并丢弃事件，从而绕开 LVGL SDL 驱动的 `lv_deinit(); exit(0)`。
- `StackView`、`Theme`、`Toast` 及工厂默认 Style 仍是进程生命周期单例；不要在一进程内重复 `initialize()`/`lv_init()`。
- 不要在 `lv_deinit()` 之后销毁仍持有 `lv_obj`/`lv_style_t` 的对象；`Application::shutdown()` 已保证正确的销毁顺序。
