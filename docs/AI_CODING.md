# AI-Assisted Development Guide for LVGL++

This document describes patterns and prompts for working on the LVGL++ framework with AI coding assistants (Claude Code, GitHub Copilot, Cursor, etc.).

## Architecture Awareness

### When the AI needs to understand project structure

```
The project root has:
- sdk/core/ — the framework library (lvglpp)
- smart_home/ — the main application
- demo/ — demo/test apps
- test/ — test app

Key architectural patterns:
1. Every widget inherits BaseItem and takes tools::LvObjFactory::createXxx as Creator fn
2. Activity lifecycle: onCreate (build UI) → onResume (show) → onPause (hide) → onDestroy
3. Navigation: StackView::getInstance().pushView/popView
4. Singleton: static T& getInstance() { static T inst; return inst; }
5. Type safety: std::any for args, unique_ptr for ownership
6. Include guards: LVGLPP_<NAME>_H
7. File extension: .cpp/.h (not .cc/.hpp)
```

### When adding a new feature

```
Rules:
- New classes go in the appropriate sdk/core/ subdirectory
- Add include guards matching the LVGLPP_* pattern
- Use std::any for type-erased data, NOT void*
- Use std::unique_ptr for owned sub-objects, NOT raw new/delete
- Do NOT add new Boost dependencies — use C++17 std:: equivalents
- Add modern alias (using ModernName = ClassName;) for new widgets
- All widgets .cpp files that use LvObjFactory need #include "core/kernel/LvObjFactory.h"
```

## Prompt Templates

### Add a new widget

```
Add a new widget class [ClassName] in sdk/core/widgets/ that inherits BaseItem.
It should:
- Take [params] in constructor
- Use tools::LvObjFactory::createXxx as the Creator function
- Follow existing naming conventions (include guard LVGLPP_*_H)
- Add a modern alias (using ModernName = ClassName;)
- Build and verify it compiles
```

### Add a new Activity page

```
Add a new Activity page [PageName] in [location].
It should:
- Inherit BaseActivity
- Override onCreate(std::any arg) to build the UI
- Use LvText for labels, PushButton/Button for buttons
- Use StackView::getInstance().pushView/popView for navigation
- Build and verify it compiles
```

### Fix a build error

```
Build the project: cmake --build build -j$(nproc)
Look at the first error and trace back to the root cause.
Fix only the minimal set of files needed.
Rebuild to verify.
```

### Refactor a widget

```
Refactor [widget] to:
- Keep the same public API (backward compatible)
- Improve internal implementation
- Ensure the Lua bindings still compile
- Build and verify
```

### Add Lua bindings for a widget

```
Add Lua bindings for [widget] in sdk/wrappers/lua/WidgetLuaBind.cc.
Follow the existing pattern:
- bind[WidgetName](sol::state&) static method
- Bind all public methods using sol's usertype
- Handle overloaded methods with static_cast
- Update WidgetLuaBind.h declaration
- Build and verify
```

## Common Pitfalls

### "tools has not been declared" / "LvObjFactory has not been declared"
```
Fix: Add #include "core/kernel/LvObjFactory.h" to the .cpp file
```

### "no match for operator=" with sol2 bindings
```
Fix: Use static_cast to resolve overloaded method:
  static_cast<void (Widget::*)(int, Enum)>(&Widget::method)
```

### "BaseItem::ItemType has not been declared"
```
Fix: Replace BaseItem(BaseItem::Type, parent) with
  BaseItem(tools::LvObjFactory::createXxx, parent)
```

### Double-free in Activity
```
Fix: Use std::unique_ptr<BaseItem> for _root_win,
  call .reset() in both destructor and onDestroy()
```

## AI Agent Interaction API

The framework includes built-in support for AI agents to inspect and control the UI:

```cpp
// Tree navigation
uint32_t count = item->childCount();
BaseItem *child = item->childAt(0);
BaseItem *found = item->findChild("name");
auto allButtons = item->findChildrenByType<PushButton>();

// State inspection
item->dumpTree();                          // print hierarchy
std::string json = item->describeState();  // JSON state
auto rect = item->bounds();                // {x, y, w, h}

// Automation
auto &ac = AutomationController::getInstance();
ac.click(item);
ac.longPress(item, 800);
ac.scrollTo(item, 0, 100);
ac.hitTest(x, y);                          // find widget at position

// Recording
auto &rec = EventRecorder::getInstance();
rec.startRecording();
// ... user interacts ...
rec.stopRecording();
rec.saveToFile("trace.json");
```

## LVGL API Quick Reference

```cpp
// Object creation
lv_obj_t *obj = lv_obj_create(parent);
lv_obj_t *label = lv_label_create(parent);
lv_obj_t *img = lv_img_create(parent);

// Styling
lv_obj_set_size(obj, w, h);
lv_obj_set_pos(obj, x, y);
lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);
lv_obj_set_style_radius(obj, 8, LV_STATE_DEFAULT);

// Text
lv_label_set_text(label, "Hello");
lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);

// Events
lv_obj_add_event_cb(obj, [](lv_event_t *e) { ... }, LV_EVENT_CLICKED, user_data);
lv_obj_send_event(obj, LV_EVENT_PRESSED, nullptr);
```
