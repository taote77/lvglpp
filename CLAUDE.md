# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LVGL++ is a C++17 object-oriented wrapper around **LVGL 9.3** (vendored at `sdk/lvgl/`) for embedded Linux touch UIs. It provides:

- `sdk/` → the `lvglpp` shared library: widget wrappers, Android-like Activity/StackView framework, asset-pack resource management, logging, and an optional Lua binding.
- `sdk/net/ws_gw/` → a standalone C++17 WebSocket gateway (POSIX sockets, thread pool, minimal in-tree JSON, no external deps), shipped as `libws_gw.so`.
- `smart_home/` → the primary application (its CMake target uses **C++20**).
- `demo/` → standalone demo executables (`demo`, `i18n_demo`, `demo-qrcode`, `remote_ctrl`, `img_disp`, optional `lua_ui`).
- `test/` → `testd` (visual app) and `test_unit` (headless unit tests).

`sdk/` is the include root (propagated publicly via the `lvgl` target), so includes are written as `#include "core/widgets/BaseItem.h"` or `#include "net/ws_gw/ws_gw.h"`.

## Build

Dependencies: CMake 3.16+, C++17 compiler (C++20 for `smart_home`), FFmpeg (`avformat/avcodec/avutil/swscale`). Platform-specific: SDL2 for the SDL backend (default on non-ARM), Linux fbdev headers for FRAMEBUF, libdrm for DRM. Optional: Lua 5.4 + pkg-config for `ENABLE_LUA`.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

Options (set at configure time):

- `-DLVGLPP_PLATFORM=AUTO|SDL|FRAMEBUF|DRM` — display/input backend. `AUTO` (default) selects SDL on non-ARM and FRAMEBUF on ARM. Only the selected backend's LVGL driver is compiled/linked.
- `-DENABLE_DEMO=OFF` — skips `demo/` only; `smart_home` and `test` are always built. Default is `ON`.
- `-DENABLE_LUA=ON` — builds the static `lvglpp_lua` target and the `lua_ui` demo (requires `lua-5.4` via pkg-config).
- `-DASAN=ON` — only takes effect when `CMAKE_BUILD_TYPE=Debug`.
- `-DUSE_MARCH_NATIVE=ON` — adds `-march=native`.

Outputs:

- Executables: `build/target/bin/` (`smart-home-ui`, `demo`, `test_unit`, `testd`, ...)
- Shared libs: `build/target/lib/` (`liblvglpp.so`, `libws_gw.so`)
- Static libs: `build/staticlib/`
- Asset packer: `build/target/bin/respack` (source in `sdk/tools/respack/`)
- Generated assets: `build/target/bin/common.pak` and `<target>.pak` (or `common.asset/` / `<target>.asset/` in directory mode)

Build a single target: `cmake --build build --target test_unit -j$(nproc)`.

### Running

```bash
cd build/target/bin
./smart-home-ui      # SDL window (needs a display/WSLg)
./testd              # visual widget test app
./test_unit          # headless
```

Run apps from `build/target/bin`: `Application::applicationDirPath()` is the **current working directory**, and assets are resolved as `<cwd>/common.pak` then `<cwd>/<exe>.pak` (directory mode uses `common.asset/` and `<exe>.asset/`). Set `LVGLPP_ASSET_DIR=<dir>` or `LVGLPP_ASSET_PACK=<pack>` to override discovery for development and tests.

### Tests

`test_unit` uses a small custom framework (`test/TestFramework.h`): `TEST(name)` registers a function, `CHECK*` macros assert, and the runner executes **all** registered tests — there is no per-test filter flag.

```bash
./build/target/bin/test_unit        # prints per-test PASSED/FAILED and a summary
ctest --test-dir build/test         # CTest integration (1 test)
```

Run CTest from `build/test`, not the build root: `enable_testing()` is called inside `test/CMakeLists.txt`, so no top-level `CTestTestfile.cmake` is generated.

Add tests in `test/UnitTests.cpp` (signals/events), `test/WidgetTests.cpp` (factory smoke tests), `test/LifecycleTests.cpp` (registry/ownership), `test/EventInputTests.cpp` (focus/key routing), `test/StyleLayoutTests.cpp` (Style/Theme/grid), `test/TimerAnimationTests.cpp`, `test/NewWidgetTests.cpp`, `test/ResourceTests.cpp` (image/memory resources), or `test/PageTests.cpp` (StackView/PageHost/PageView/TabView). Widget tests use the display created in `test/TestMain.cpp`; new test files must be added to the explicit list in `test/CMakeLists.txt`.

### Formatting / lint

`.clang-format` and `.clang-tidy` exist at the repo root, but there are no CMake targets for them:

```bash
clang-format -i <file>                                     # apply style
clang-format --dry-run --Werror <file>                     # check only
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON      # needed for clang-tidy
clang-tidy -p build <file>
```

## Architecture

### Widget layer and ownership (`sdk/core/widgets`, `sdk/core/base`)

- `core::Object` provides named signals/slots (`connect`, `emitSignal`, `disconnect`) and a string property map. It has no LVGL dependency.
- `widgets::BaseItem` wraps an `lv_obj_t*`. Every widget passes a `Creator` (`tools::LvObjFactory::createXxx`) to `BaseItem`. Constructors take the parent as the **last argument**; the parent pointer is non-owning.
- Children are owned by the LVGL parent; child `BaseItem` wrappers are **non-owning handles** and may be destroyed while their `lv_obj` keeps living. Top-level roots and layer-attached objects (`BaseDialog`/`Toast` creators that attach to `lv_layer_top()/sys()`) own and delete their `lv_obj`. `ownsLvglObject()` reports which case applies.
- A global `lv_obj -> wrapper` registry plus an `LV_EVENT_DELETE` watchdog makes lifetime safe: wrapper destruction removes all its LVGL event callbacks, detaches it from focus groups, and an externally deleted tree nulls `lv_base_ptr_` so later calls are no-ops. `childAt()`/`findChild()`/`dumpTree()`/`AutomationController::hitTest()` resolve wrappers through this registry (not `lv_obj` user_data).
- `BaseItem` and `core::Object` are non-copyable and non-movable.
- LVGL retains caller-owned raw pointers (grid descriptor arrays, image paths, ImageButton sources, styles). Use `BaseItem::setObjectKeepalive(key, shared_ptr)` so they survive short-lived wrappers.
- **Callback-bearing widgets must stay alive**: event handlers are removed together with their wrapper. Keep interactive widgets in members or `widgets::WidgetStore` (`sdk/core/widgets/WidgetStore.h`). Short-lived locals are only appropriate for non-interactive visual widgets.
- Events: prefer `BaseItem::onClicked/onKey/onFocus/...` (returns `core::Connection`; use `core::ScopedConnection` for RAII) or `addLvglEventHandler()`. Legacy `setOnClickedListener` still works. `Object` receiver-tracked connections auto-disconnect when the receiver dies.
- `LvObjFactory` has one static creator per LVGL widget type. New widgets must add a creator there and pass it to `BaseItem`; interactive widgets should call `setFocusable(true)`.
- Modern aliases exist for several widgets: `Label = LvText`, `Button = PushButton`, `Chart = LvChart`, `TextButton = LvButton`, `Bar = ProgressBar`.
- Template widgets use the delegate pattern: `ListView.hpp`, `PageView.hpp` with `ListViewDelegate.hpp` / `PageViewDelegate.hpp`.

### Application framework (`sdk/core/sys`)

- `sys::Application` is concrete and composes a `sys::Platform` backend (`core/sys/platform/Platform.h`); platform types are not part of the application hierarchy. Backends: `SdlPlatform`, `FramebufPlatform`, `DrmPlatform` in `core/sys/platform/`. A new backend only needs to implement `Platform` and register in `PlatformFactory.cpp`; `WaylandApplication` was removed.
- `createApplication(argc, argv)` (`core/sys/PlatformFactory.h`) creates the compiled-in backend. CMake option `LVGLPP_PLATFORM=AUTO|SDL|FRAMEBUF|DRM` (default `AUTO`: SDL on non-ARM, FRAMEBUF on ARM) defines exactly one `LVGLPP_PLATFORM_*` macro; `sdk/lv_conf.h` maps it to `LV_USE_SDL` / `LV_USE_LINUX_FBDEV` / `LV_USE_LINUX_DRM` / `LV_USE_EVDEV`.
- Lifecycle: `Application::initialize()` → `lv_init()`, asset mounts + `A:` driver, LVGL log hook, SIGINT/SIGTERM handlers, ThorVG init, `platform_->init()`, tick fallback, `InputManager::install()`, then the `onInit()` hook. `exec()` initializes if needed, runs the loop and returns the exit code. `shutdown()` is idempotent and ordered: `onShutdown()` → tick thread join → `StackView::shutdown()` → `ImageManager::shutdown()` → `InputManager::shutdown()` → platform shutdown → `lv_deinit()` → asset reset.
- `Application::exec()` drains the whole event queue per iteration, calls `lv_timer_handler()`, and sleeps `min(time-to-next-timer, 5 ms)`. LVGL/UI work is single-threaded.
- `requestQuit(code)` is thread-safe and the first code wins. SIGTERM/SIGINT produce exit codes 143/130. `SdlPlatform` installs an SDL event filter that turns `SDL_QUIT`/`SDL_WINDOWEVENT_CLOSE` into `requestQuit()` and drops the event, so LVGL's SDL driver `exit(0)` path is never taken.
- Background threads must not touch widgets. Wrap data in `sys::Event` and call `Application::postEvent(...)`; `StackView::notifyAllUi()` delivers it to every Activity's `onNotifyUI(const Event&)` on the UI thread.
- `Activity` lifecycle mirrors Android: `onCreate(std::any arg)` → `onResume()` → `onPause()` → `onDestroy()`. `BaseActivity` owns a root `BaseItem` in `std::unique_ptr`.
- `sys::InputManager` (`core/sys/InputManager.{h,cpp}`) owns keyboard focus scopes, a virtual keypad indev and global key routing. `BaseActivity::onResume/onPause` push/pop an Activity scope; `BaseDialog::open/close` push/pop a modal scope (ESC closes the dialog by default). Interactive widgets call `setFocusable(true)` and expose `onKey()`; unhandled keys bubble to ancestors and then global handlers. Physical/custom keys are injected thread-safely with `postKey()/postKeyRaw()`.
- `core::Style`/`core::Theme` (`sdk/core/style/`) provide shared styles, semantic color roles (dark/light) and the focus ring. Styles are immutable once attached; `BaseItem::applyStyle()` keeps them alive via the registry.
- Layout helpers on `BaseItem`: flex plus a working grid descriptor parser (`"1fr 2fr auto 100px"`); `getWidth()/getHeight()` return live layout dimensions with a requested-size fallback.
- Navigation via the `StackView` singleton: `pushView(std::shared_ptr<Activity>, arg)`, `popView()`, `pushViewAndReplaced(...)`, `popViewImmediately()`. `pushView` calls before `exec()` are queued with their arguments; `StackView::start()` initializes every queued view in stack order and leaves only the top resumed. Pop transitions are serialized: navigation during an animation is queued instead of re-entering. Popped views are destroyed asynchronously via `clearDeleteVec()` in the main loop.
- After manually toggling page/tab visibility inside an Activity, call `BaseActivity::refreshFocus()` so the keyboard focus group is rebuilt from the currently visible widgets. `widgets::PageHost` does this automatically.
- `widgets::PageHost` (`core/widgets/PageHost.{h,cpp}`) manages lazy-loaded pages inside one container: `addPage(name, builder)`, `show(index)`, `PageContext::emplace/onShow/onHide/onDestroy`, and `CachePolicy::{KeepAll,ReleaseOnHide,MaxCached}`. Pages get a per-page `WidgetStore` and are released with their callbacks. smart_home's five tabs use it; only the first page is built at startup.
- `TabView::addTab()` returns `std::unique_ptr<BaseItem>` (non-owning page handle); `TabView::content(idx)` returns the actual page `lv_obj_t*`. `PageView` cancels pending async delegate creation when the wrapper/lv_obj dies and clamps its current index.
- `BaseActivity` roots are created with no parent, so they attach to `lv_scr_act()`.

### Resources and assets

- Assets live in a top-level `assets/` tree: `assets/common/` (shared fonts, images, Lottie) and optional `assets/apps/<target>/` (app-specific extras). Logical paths are relative and `/`-separated, e.g. `image/40x40/icon_warn.png`, `fonts/NotoSansSC-Regular.ttf`, `lottie/camera.json`.
- `sdk/tools/respack` packs a directory into an `LVPAK1` file. The format has a 40-byte header, 8-byte-aligned raw data and a sorted index with CRC-32 per entry and for the index:
  ```bash
  respack pack <asset-dir> <out.pak> [--exclude <glob>]...
  respack list <in.pak>
  respack verify <in.pak>
  ```
- `tools::AssetManager` mounts one or more sources (pack, directory or in-memory pack); the last mounted source wins. `Application::initialize()` mounts `common.pak` first and `<exe>.pak` second. The `A:` LVGL FS driver (`FileSystemManager::registerAssetDriver()`) exposes the same files read-only for controllers that need a file path (GIF, fonts, ...). `AssetManager::materialize()` extracts a pack entry to a per-process temp file for libraries that bypass LVGL's FS (FreeType without the LVGL port, ffmpeg).
- `tools::ImageManager` decodes asset bytes with `stb_image` into LVGL ARGB8888 (BGRA) pixels wrapped in `ImageResource`. `load(assetPath)` is synchronous; `prefetch(...)` warms the cache on a background worker (never touches LVGL); `waitForPrefetch()` joins the queue. The cache is a mutex-protected **safe LRU** (defaults 32 entries / 64 MB); eviction drops only the cache reference, while `Image` keeps an `ImageHandle` via the `BaseItem` registry keepalive. Dimensions are validated (≤16384², ≤256 MB). `loadFile()` handles real filesystem images; `invalidate()`/`Image::reload()` support directory-mode hot reload.
- `Image` treats its string constructor argument as an asset path; use `Image(FileSource{path}, parent)` for real files. `ImageButton` resolves all three parts through `ImageManager` and keepalives the descriptors; `LottieCanvas` uses `lv_lottie_set_src_data()` for packed JSON; `GifImage` reads through `A:`.
- CMake option `LVGLPP_ASSET_MODE=pack|directory|both` (default `pack`). The helper `lvglpp_attach_assets(<target>)` stages the common assets plus `assets/apps/<target>/`; packs are generated at build time with `respack`.

### WebSocket gateway (`sdk/net/ws_gw`)

Standalone library (does not depend on lvglpp/LVGL). Public API in `ws_gw.h`:

```cpp
WsGwSetting setting;
setting.bind_ip = "0.0.0.0";
setting.port = 9998;
setting.callback = [](const std::string &id, wsgw::Json &req, wsgw::Json &res) { /* handle */ };
WsGwContext *ctx = ws_gw_start(setting);
ws_gw_send_nofity(ctx, id, msg);   // note the existing spelling
```

`TcpListener` accepts connections and queues them to worker threads; each `WsSession` performs the HTTP upgrade, frame parsing/writing, and invokes the user callback. `wsgw::Json` is a dependency-free JSON subset. `demo/remote_ctrl` is the reference consumer.

### Lua bindings (`sdk/wrappers/lua`) — optional

Built only with `-DENABLE_LUA=ON`, producing static `lvglpp_lua` using sol2 (headers vendored in `demo/clua/include`). `WidgetLuaBind.cc` binds `BaseItem` and widget subclasses; `LvglLuaBind.cc` owns the Lua state/root widget. See `docs/LUA_BINDING_SUMMARY.md`.

### smart_home

- `smart_home/main.cc` defines `SmartHomeShell : BaseActivity`. It uses `widgets::PageHost` for five lazy tab pages (only the first page is built at startup; visited pages are kept alive by default) and swaps pages on nav-button clicks.
- `smart_home/CMakeLists.txt` uses an **explicit source list**. New `.cc` files must be added there. Only the components plus `page/sensor` and `page/drawer` are currently compiled; the `page/home`, `page/devices`, `page/energy`, `page/rooms`, `page/scenes`, and `page/settings` directories exist but are not part of the target.
- i18n: `i18n/LvTranslator.{h,cc}` plus generated `i18n/translations/lv_i18n.{c,h}`; source YAML and helper scripts (`compile.sh`, `extract.sh`, `merge_i18n.py`, ...) live in `i18n/translations/`. `LvTranslator::Instance().init()/load(LANGUAGE::ENGLISH)` runs in `main`.
- Theme/colors: `sdk/config/color_sheet.h` defines `form::design::CLR_*` constants; `smart_home/theme/` adds `form_design*.h` and `ThemeKeeper`.

## Coding Conventions

- File extension `.cpp`/`.h` (legacy `.cc` remains in `sdk/wrappers/lua`); include guards `LVGLPP_<NAME>_H`.
- Namespaces: `lvglpp::core`, `lvglpp::widgets`, `lvglpp::sys`, `lvglpp::tools`, `lvglpp::db`.
- Use `std::any` for type-erased data, never `void*`; prefer `std::unique_ptr` for owned sub-objects and raw pointers for non-owning parents.
- Do not introduce Boost; use C++17 `std::` equivalents (any, optional, filesystem, string_view).
- Singletons use Meyer's pattern: `static T &getInstance() { static T inst; return inst; }`.
- Every widget `.cpp` that uses the factory must `#include "core/base/LvObjFactory.h"` (the path is `base`, not the older `kernel`).
- Adding a widget: inherit `BaseItem`, add `tools::LvObjFactory::createXxx`, pass it to the `BaseItem` constructor, add a modern alias, call `setFocusable(true)` for interactive widgets, and update `sdk/wrappers/lua/WidgetLuaBind.cc` if Lua support is required.
- Register events through `onClicked/onKey/addLvglEventHandler`; never call `lv_obj_add_event_cb(..., this)` directly.
- Store callback-bearing widgets in members or `widgets::WidgetStore`; wrapper destruction removes its callbacks.
- The full SDK guide is `docs/CPP_SDK_GUIDE.md`.

## Known Gotchas

- Wrapper destruction removes its event handlers. A widget created in a local scope becomes inert after the scope ends even though its `lv_obj` remains visible; keep callback-bearing widgets in members or `WidgetStore`.
- `childAt()` only returns children that still have a live wrapper. Children created through short-lived wrappers (or LVGL-internal children) resolve to `nullptr`; use `findChild()`/`findChildrenByType()` if you need traversal to skip unwrapped intermediates.
- `getParent()` walks the LVGL parent chain and returns the nearest live wrapper ancestor, not a stored pointer. `setParent()` also updates ownership.
- `getWidth()/getHeight()` return live layout sizes; before the first layout they fall back to the size requested via `setSize()`.
- `Style` becomes immutable on first application. Use `Style::clone()` for variants. Theme changes do not restyle existing widgets; set `Theme::setMode()` before building the UI.
- SDL keyboard non-Enter releases are synthesized (LVGL only delivers full release semantics for Enter). Use `InputManager::postKey()` for hardware keys that need real press/release.
- `test_unit` creates a headless display with a no-op flush callback; without one, `lv_timer_handler()` blocks in LVGL's `wait_for_flushing()`.
- The LVGL SDL driver still calls `lv_deinit(); exit(0)` on `SDL_QUIT`, but `SdlPlatform` installs an SDL event filter that intercepts `SDL_QUIT`/`SDL_WINDOWEVENT_CLOSE` and requests a controlled quit instead. `Application` also handles SIGINT/SIGTERM. `StackView`, `Theme`, `Toast` and the factory default styles remain process-lifetime singletons. Call `Application::shutdown()` (or `requestQuit()` + `exec()`) for a graceful teardown.
- Assets are served from `common.pak` + `<exe>.pak`. A missing logical path logs **once** per path. For hot reload, run with `LVGLPP_ASSET_DIR=<dir>` (or build with `-DLVGLPP_ASSET_MODE=directory`) and call `ImageManager::invalidate()` / `Image::reload()` after replacing files.
- LVGL config is `sdk/lv_conf.h`, which includes `sdk/config/lv_conf_my.h` (screen size, SDL/DRM, Lottie, etc.). Display dimensions are `800x480` by default.
- `.gitmodules` points at a root-level `lvgl` path, but LVGL is vendored directly under `sdk/lvgl/` as tracked files (9.3.0). Do not expect `git submodule update` to populate `sdk/lvgl`.
- Most demos and real platform backends require a graphical display; only `test_unit` runs headless.
