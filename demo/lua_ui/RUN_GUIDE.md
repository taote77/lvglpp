# LVGL Lua UI 示例运行指南

## 快速开始

### 1. 编译项目

```bash
cd /home/shane/workspace/lvglpp
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### 2. 运行示例

```bash
cd /home/shane/workspace/lvglpp/build/target/bin
./lua_ui
```

## 示例说明

当前示例 (`simple_demo.lua`) 创建了一个简单的 UI 界面，包含：

- 标题文本："LVGL Lua Demo"
- 副标题文本："Build UI with Lua scripts"
- 两个按钮：
  - "Hello Lua!" - 点击后显示 "Hello from Lua!"
  - "Click Me" - 点击后显示 "Button clicked!"
- 状态文本："Ready - Try clicking to controls!"

## Lua 脚本示例

```lua
-- Simple Lua UI Demo
-- 这个示例展示了如何用 Lua 创建一个简单的 UI 界面

lvgl.print("=== Simple Lua UI Demo ===")

-- 创建标题
local title = lvgl.create_text("LVGL Lua Demo", 0xFFFFFF, root)
title:set_pos(50, 30)

-- 创建副标题
local subtitle = lvgl.create_text("Build UI with Lua scripts", 0xCCCCCC, root)
subtitle:set_pos(50, 80)

-- 创建一个蓝色按钮
local button1 = lvgl.create_button(200, 50, "Hello Lua!", root)
button1:set_pos(50, 150)
button1:set_on_clicked(function()
    lvgl.print("Hello button clicked!")
    subtitle:set_text("Hello from Lua!")
end)

-- 创建一个红色按钮
local button2 = lvgl.create_button(200, 50, "Click Me", root)
button2:set_pos(300, 150)
button2:set_on_clicked(function()
    lvgl.print("Click Me button clicked!")
    subtitle:set_text("Button clicked!")
end)

-- 创建状态文本
local status = lvgl.create_text("Ready - Try clicking to controls!", 0x888888, root)
status:set_pos(50, 350)

lvgl.print("=== UI setup completed! ===")
```

## 支持的 Lua API

### 创建控件

```lua
-- 创建文本
local text = lvgl.create_text("Hello", 0xFFFFFF, root)

-- 创建按钮
local button = lvgl.create_button(width, height, text, root)

-- 创建图片
local image = lvgl.create_image(path, root)
```

### 控件方法

```lua
-- 位置和大小
widget:set_pos(x, y)
widget:set_size(width, height)

-- 颜色和样式
widget:set_bg_color(color)
widget:set_opacity(opacity)

-- 可见性和启用
widget:set_visible(true)
widget:set_enable(true)

-- 事件处理
widget:set_on_clicked(function()
    -- 点击事件处理
end)

widget:set_on_pressed(function()
    -- 按下事件处理
end)

widget:set_on_released(function()
    -- 释放事件处理
end)
```

### 工具函数

```lua
lvgl.print("Message")  -- 打印消息
lvgl.log("Log")       -- 记录日志
```

## 颜色格式

颜色使用十六进制格式：
- `0xFFFFFF` - 白色
- `0x000000` - 黑色
- `0xFF0000` - 红色
- `0x00FF00` - 绿色
- `0x0000FF` - 蓝色
- `0xCCCCCC` - 灰色

## 运行结果

成功运行后，你应该能看到：

1. 控制台输出：
   ```
   [Lua Info] LvglLuaBind initialized successfully
   Lua UI Application initialized
   Loading Lua script...
   [Lua] === Simple Lua UI Demo ===
   [Lua] === UI setup completed! ===
   Lua script loaded successfully
   ```

2. 图形界面：
   - 一个 800x480 的窗口
   - 标题和副标题
   - 两个按钮
   - 状态文本

3. 交互：
   - 点击按钮会更新状态文本
   - 控制台会显示点击事件

## 故障排除

### 编译错误

如果遇到编译错误，确保：
- 已安装 Lua 开发库：`sudo apt-get install liblua5.2-dev`
- 已安装 sol 库（已包含在 demo/clua/include）

### 运行时错误

如果遇到运行时错误：
- 检查 Lua 脚本语法
- 查看控制台输出的错误信息
- 确保所有控件都已正确创建

### 显示问题

如果界面不显示：
- 检查窗口大小设置
- 确保控件位置在窗口范围内
- 检查控件可见性设置

## 下一步

- 查看 [QUICKSTART.md](QUICKSTART.md) 了解更多用法
- 查看 [README.md](../sdk/lua/README.md) 获取完整 API 文档
- 创建你自己的 Lua UI 脚本
- 扩展绑定以支持更多控件

## 技术支持

- LVGL 版本：v9
- Lua 版本：5.2
- C++ 标准：C++17
- 构建系统：CMake 3.16+

## 许可证

本项目遵循与 LVGL C++ SDK 相同的许可证。
