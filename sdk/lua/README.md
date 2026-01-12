# LVGL C++ SDK Lua 绑定

这是一个为 LVGL C++ SDK 提供的完整 Lua 接口绑定，让开发者可以用 Lua 脚本创建和管理 UI 界面。

## 功能特性

- 完整的 Lua API 绑定，支持常用的 LVGL 控件
- 基于现代 C++17 和 sol 库
- 支持链式调用
- 事件回调支持
- 易于扩展

## 支持的控件

- **BaseItem** - 基础控件
- **LvText** - 文本控件
- **Image** - 图片控件
- **PushButton** - 按钮控件
- **CheckBox** - 复选框控件
- **SwitchButton** - 开关按钮控件

## 编译

### 前置要求

- CMake 3.16+
- C++17 编译器
- LVGL 库
- sol 库（已包含在 demo/clua/include 目录）

### 编译步骤

```bash
mkdir build && cd build
cmake ..
make -j4
```

编译完成后，可执行文件位于 `build/demo/lua_ui/lua_ui`

## 使用方法

### C++ 代码

```cpp
#include "core/sys/SdlApplication.h"
#include "lua/LvglLuaBind.h"

using namespace lvglpp;
using namespace lvglpp::lua;

class LuaUIApplication : public sys::SdlApplication {
public:
    LuaUIApplication(int argc, char* argv[])
        : SdlApplication(argc, argv)
        , lua_bind_(std::make_unique<LvglLuaBind>()) {
    }

protected:
    bool initApp() override {
        if (!SdlApplication::initApp()) {
            return false;
        }

        lua_bind_->initialize();

        auto root = std::make_shared<widgets::BaseItem>(nullptr);
        root->setSize(800, 480);
        lua_bind_->setRootItem(root);

        lua_bind_->executeFile("example.lua");
        return true;
    }

private:
    std::unique_ptr<LvglLuaBind> lua_bind_;
};

int main(int argc, char* argv[]) {
    LuaUIApplication app(argc, argv);
    return app.exec();
}
```

### Lua 脚本

```lua
-- 创建文本
local text = lvgl.create_text("Hello Lua!", 0xFFFFFF, root)
text:set_pos(50, 50)

-- 创建按钮
local button = lvgl.create_button(200, 50, "Click Me", root)
button:set_pos(50, 100)
button:set_on_clicked(function()
    print("Button clicked!")
end)

-- 创建复选框
local checkbox = lvgl.create_checkbox(30, 30, 0x333333, 0x00AAFF, root)
checkbox:set_pos(50, 200)

-- 创建开关
local switch = lvgl.create_switch(64, 32, root)
switch:set_pos(200, 200)
```

## Lua API 参考

### 创建控件

#### lvgl.create_text(text, color, parent)
创建一个文本控件。

**参数:**
- `text` (string) - 文本内容
- `color` (number) - 文本颜色（十六进制，如 0xFFFFFF）
- `parent` (BaseItem) - 父控件

**返回:** LvText 对象

#### lvgl.create_text_ex(text, color, parent, style, font_type)
创建一个带样式的文本控件。

**参数:**
- `text` (string) - 文本内容
- `color` (number) - 文本颜色
- `parent` (BaseItem) - 父控件
- `style` (LvText.TextStyle) - 文本样式
- `font_type` (LvText.FontType) - 字体类型

**返回:** LvText 对象

#### lvgl.create_image(path, parent)
创建一个图片控件。

**参数:**
- `path` (string) - 图片路径
- `parent` (BaseItem) - 父控件

**返回:** Image 对象

#### lvgl.create_button(width, height, text, parent)
创建一个按钮控件。

**参数:**
- `width` (number) - 按钮宽度
- `height` (number) - 按钮高度
- `text` (string) - 按钮文本
- `parent` (BaseItem) - 父控件

**返回:** PushButton 对象

#### lvgl.create_button_ex(width, height, color_style, text, parent)
创建一个带样式的按钮控件。

**参数:**
- `width` (number) - 按钮宽度
- `height` (number) - 按钮高度
- `color_style` (PushButton.ColorStyle) - 按钮样式
- `text` (string) - 按钮文本
- `parent` (BaseItem) - 父控件

**返回:** PushButton 对象

#### lvgl.create_checkbox(width, height, bg_color, check_color, parent)
创建一个复选框控件。

**参数:**
- `width` (number) - 宽度
- `height` (number) - 高度
- `bg_color` (number) - 背景颜色
- `check_color` (number) - 选中颜色
- `parent` (BaseItem) - 父控件

**返回:** CheckBox 对象

#### lvgl.create_switch(width, height, parent)
创建一个开关按钮控件。

**参数:**
- `width` (number) - 宽度
- `height` (number) - 高度
- `parent` (BaseItem) - 父控件

**返回:** SwitchButton 对象

### 通用方法

所有控件都支持以下方法：

- `set_pos(x, y)` - 设置位置
- `get_x()` - 获取 X 坐标
- `get_y()` - 获取 Y 坐标
- `set_alignment(align, offset_x, offset_y)` - 设置对齐方式
- `set_alignment_to(item, align, offset_x, offset_y)` - 相对对齐
- `set_bg_color(color)` - 设置背景颜色
- `set_opacity(opacity)` - 设置透明度 (0.0-1.0)
- `set_size(width, height)` - 设置大小
- `set_width(width)` - 设置宽度
- `get_width()` - 获取宽度
- `set_height(height)` - 设置高度
- `get_height()` - 获取高度
- `set_visible(visible)` - 设置可见性
- `get_visible()` - 获取可见性
- `set_enable(enable)` - 设置启用状态
- `get_enable()` - 获取启用状态
- `set_radius(radius)` - 设置圆角半径
- `get_radius()` - 获取圆角半径
- `set_clip(clip)` - 设置裁剪
- `set_border(width, color)` - 设置边框
- `set_padding_bottom(padding)` - 设置底部内边距
- `set_padding_top(padding)` - 设置顶部内边距
- `set_padding_hor(padding)` - 设置水平内边距
- `set_parent(parent)` - 设置父控件
- `get_parent()` - 获取父控件

### 事件处理

- `set_on_clicked(func)` - 设置点击事件回调
- `set_on_pressed(func)` - 设置按下事件回调
- `set_on_released(func)` - 设置释放事件回调

### LvText 特有方法

- `set_text(text)` - 设置文本内容
- `get_text()` - 获取文本内容
- `set_text_color(color)` - 设置文本颜色
- `set_text_align(align)` - 设置文本对齐
- `set_recolor_enable(enable)` - 启用重着色
- `set_max_len(len)` - 设置最大长度
- `get_display_text_width()` - 获取显示文本宽度
- `set_long_mode(max_width, mode)` - 设置长文本模式

### Image 特有方法

- `set_image_src_path(path)` - 设置图片路径
- `get_image_src_path()` - 获取图片路径

### PushButton 特有方法

- `set_image_pos(x, y)` - 设置图片位置
- `set_text_pos(x, y)` - 设置文本位置
- `set_text_alignment(align)` - 设置文本对齐
- `set_text_msg(text)` - 设置按钮文本
- `change_image_url(url)` - 更改图片 URL
- `set_color_type(type)` - 设置颜色类型
- `set_repeat_ms(ms)` - 设置重复间隔

### CheckBox 特有方法

- `get_check_state()` - 获取选中状态

### SwitchButton 特有方法

- `set_check(checked)` - 设置开关状态
- `is_check()` - 获取开关状态

## 示例

查看 `demo/lua_ui/example.lua` 获取完整示例。

## 扩展

要添加新的控件绑定：

1. 在 `WidgetLuaBind.cpp` 中添加新的绑定函数
2. 在 `LvglLuaBind.cpp` 的 `registerLuaAPI()` 中添加创建函数
3. 更新文档

## 许可证

本项目遵循与 LVGL C++ SDK 相同的许可证。
