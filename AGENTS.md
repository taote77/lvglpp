# AGENTS.md — AI Coding Assistant Guide for LVGL++

This file provides concise instructions for AI coding assistants working on this project.

## Quick Reference

- **Language**: C++17
- **Build**: `cmake --build build -j$(nproc)` (configure: `cmake .. -DENABLE_DEMO=OFF`)
- **Library**: `lvglpp` (shared), `lvglpp_lua` (static Lua bindings)
- **LVGL version**: 9.x (submodule at `sdk/lvgl/`)

## Code Rules (MUST FOLLOW)

1. **File extension**: `.cpp` / `.h` — never `.cc` or `.hpp`
2. **Include guards**: `LVGLPP_<NAME>_H` — e.g. `#ifndef LVGLPP_MYWIDGET_H`
3. **No void\***: Use `std::any` for type-erased data
4. **No raw ownership**: Use `std::unique_ptr<T>`, never raw `new`/`delete` for owned objects
5. **No Boost in sdk/core/**: Use C++17 `std::` equivalents (any, optional, filesystem, string_view)
6. **Singleton**: `static T& getInstance() { static T inst; return inst; }`
7. **Widget .cpp must include**: `#include "core/kernel/LvObjFactory.h"`

## Architecture (MUST FOLLOW)

- Widget inherits `BaseItem(BaseItem::Creator, parent)` — Creator = `tools::LvObjFactory::createXxx`
- Activity inherits `sys::BaseActivity`, override `onCreate(std::any arg)`
- Navigation: `StackView::getInstance().pushView(...)` / `popView()`
- Add aliases for new widgets: `using ModernName = ClassName;`

## After Every Change

1. Build and verify: `cmake --build build -j$(nproc)`
2. Fix errors before declaring completion
