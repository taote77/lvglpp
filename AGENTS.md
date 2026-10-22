# AGENTS.md — AI Coding Assistant Guide for LVGL++

This file provides concise instructions for AI coding assistants working on this project.

## Quick Reference

- **Language**: C++17
- **Build**: `cmake --build build -j$(nproc)` (configure: `cmake .. -DENABLE_DEMO=OFF`, add `-DENABLE_LUA=ON` to enable Lua, `-DLVGLPP_ENABLE_FREETYPE=ON` to enable FreeType; it is **off** by default)
- **Orientation/RTL**: `-DLVGLPP_ENABLE_ROTATION=ON|OFF` / `-DLVGLPP_ENABLE_RTL=ON|OFF` (both default ON). Set both OFF for a zero-overhead static UI.
- **Library**: `lvglpp` (shared), `lvglpp_lua` (static Lua bindings)
- **LVGL version**: 9.x (vendored at `sdk/lvgl/`)
- **Fonts**: compiled-in bitmap subsets in `sdk/core/tools/fonts/`; regenerate with `sdk/tools/i18n/gen_fonts.py` after changing `smart_home/i18n/strings.xlsx`
- **i18n**: Excel is the source of truth; `gen_lv_i18n.py` generates `lv_i18n.c`; use `lvTr("strid")` / `lvTrFmt(...)` in UI code

## Code Rules (MUST FOLLOW)

1. **File extension**: `.cpp` / `.h` — never `.cc` or `.hpp`
2. **Include guards**: `LVGLPP_<NAME>_H` — e.g. `#ifndef LVGLPP_MYWIDGET_H`
3. **No void\***: Use `std::any` for type-erased data
4. **No raw ownership**: Use `std::unique_ptr<T>`, never raw `new`/`delete` for owned objects
5. **No Boost in sdk/core/**: Use C++17 `std::` equivalents (any, optional, filesystem, string_view)
6. **Singleton**: `static T& getInstance() { static T inst; return inst; }`
7. **Widget .cpp must include**: `#include "core/base/LvObjFactory.h"`
8. **Event callbacks**: use `BaseItem::onClicked/onKey/...` or `addLvglEventHandler()`; never register raw callbacks with `this` as user_data
9. **Widget lifetime**: callbacks die with their wrapper. Store callback-bearing widgets in a member or `widgets::WidgetStore`
10. **BaseItem is non-copyable/non-movable**; child wrappers are non-owning handles

## Architecture (MUST FOLLOW)

- Widget inherits `BaseItem(BaseItem::Creator, parent)` — Creator = `tools::LvObjFactory::createXxx`
- Activity inherits `sys::BaseActivity`, override `onCreate(std::any arg)`
- Navigation: `StackView::getInstance().pushView(...)` / `popView()`
- Add aliases for new widgets: `using ModernName = ClassName;`
- Focus/keyboard handling: `sys::InputManager` (automatic per-Activity and per-dialog scopes)
- Styling: `core::Style` / `core::Theme`; layout: flex/grid helpers on `BaseItem`
- Configuration changes: `sys::UiConfig` + `Activity::onConfigurationChanged`; rebuild pages via `PageHost::releaseAll()+show(current)`
- RTL: `BaseItem` mirrors explicit align/pos; **do not** manually mirror flex/grid (LVGL reverses them); charts are LTR
- Full usage guide: `docs/CPP_SDK_GUIDE.md`

## After Every Change

1. Build and verify: `cmake --build build -j$(nproc)`
2. Fix errors before declaring completion
