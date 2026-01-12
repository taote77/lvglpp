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
local button1 = lvgl.create_button(200, 50, "Hello Lua1!", root)
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
