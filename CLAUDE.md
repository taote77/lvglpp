# LVGL++ — C++ Framework for LVGL

C++17 object-oriented wrapper around LVGL 9.x, designed for embedded Linux GUI applications.

## Architecture

```
sdk/
├── config/        lv_conf.h, color_sheet.h
├── core/
│   ├── base/      Object, LvObjFactory        — base classes & widget factory
│   ├── sys/       Activity, Application, StackView, Event, Timer, FileSystemManager
│   │   ├── app/       Sdl/Drma/Framebuf/Wayland Application subclasses
│   │   └── automation/  AutomationController, EventRecorder
│   ├── widgets/   BaseItem + 20+ widget subclasses (LvText, PushButton, Image, ListView, ...)
│   ├── tools/     Utils, ImageManager, MemoryResManager, fonts, stb, base64
│   ├── db/        AssetResDataBase (SQLite)
│   └── log/       Boost.Log wrapper (hglv::Logger)
├── wrappers/
│   └── lua/       Lua bindings (sol2)
├── net/ws_gw/     WebSocket gateway (Boost.Beast)
└── lvgl/          LVGL submodule (9.x)
smart_home/
├── components/    Reusable UI widgets (ClickableCard, GroupCheckButton, LineEditor, ...)
├── page/          Pages: home/, sensor/, drawer/, settings/
├── i18n/          Translations
└── theme/         Theme definitions (light/dark)
demo/              Demo applications (i18n, qrcode, remote_ctrl, etc.)
test/
├── testd          Visual test app
└── test_unit      Unit tests (17 tests, headless)
docs/              Documentation (AI_CODING.md, LUA_BINDING_SUMMARY.md)
.claude/skills/    Claude Code skills (add-widget, add-page, build-check, lua-bind, audit)
```

## Key Patterns

### Widget Creation (Factory Pattern)
```cpp
// Each widget passes its LVGL creator function to BaseItem's constructor
class LvText : public BaseItem {
    LvText(...) : BaseItem(tools::LvObjFactory::createLvglLabel, parent) {}
};
class Image : public BaseItem {
    Image(...) : BaseItem(tools::LvObjFactory::createLvglImage, parent) {}
};
// Modern aliases available:
using Label = LvText;
using Button = PushButton;
using Chart = LvChart;
```

### Activity Lifecycle (Android-like)
```cpp
class MyPage : public sys::BaseActivity {
    void onCreate(std::any arg) override;  // build UI
    void onResume() override;              // shown
    void onPause() override;               // hidden
    void onDestroy() override;             // cleanup
};
// Navigation:
StackView::getInstance().pushView(std::make_shared<MyPage>(), arg);
StackView::getInstance().popView();
```

### AI Agent Interaction (Introspection + Automation)
```cpp
rootItem->dumpTree();                                // print widget hierarchy
auto *btn = rootItem->findChild("btn_ok");           // find by name
btn->describeState();                                // JSON state snapshot
auto &ac = AutomationController::getInstance();
ac.click(btn);                                       // simulate click
ac.clickAt(100, 200);                                // click at coordinates
EventRecorder::getInstance().startRecording();        // record interactions
```

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEMO=OFF
make -j$(nproc)
```

Dependencies: CMake 3.16+, C++17, LVGL 9.x, Boost (log, filesystem, thread, json), SDL2, SQLite3, FFmpeg, Lua 5.4

## Coding Conventions

- **File extension**: `.cpp` / `.h` (unified from .cc)
- **Include guards**: `LVGLPP_<NAME>_H` pattern
- **Namespace**: `lvglpp::widgets`, `lvglpp::sys`, `lvglpp::tools`, `lvglpp::core`
- **Smart pointers**: Prefer `std::unique_ptr` for ownership, raw pointer `BaseItem*` for parent references (LVGL manages lifecycle)
- **Type safety**: Use `std::any` (not `void*`) for lifecycle args and extra data
- **Don't use**: `boost::any`, `boost::optional`, `boost::filesystem`, `boost::format` — use C++17 `std::` equivalents
- **Singleton**: Use Meyer's singleton pattern (`static T& instance()`)
- **Widget constructor**: Pass `tools::LvObjFactory::createXxx` as the `Creator` function pointer to `BaseItem`

## Module Dependencies

- `lvgl` — LVGL C library (static)
- `lvglpp` — main shared library (depend on it for all widget/app code)
- `lvglpp_lua` — Lua bindings (static, optional)
- `ws_gw` — WebSocket gateway (shared, optional)

## When Adding a New Widget

1. Inherit from `BaseItem` (preferred) or `LvText` (for text-based widgets)
2. Pass the appropriate `tools::LvObjFactory::createXxx` to `BaseItem` constructor
3. Add modern alias (`using ModernName = ClassName;`) for clean API
4. Update `sdk/wrappers/lua/WidgetLuaBind.cc` if Lua bindings needed
5. Include `"core/kernel/LvObjFactory.h"` in the `.cpp` file
