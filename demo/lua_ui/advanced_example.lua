-- Advanced Lua UI Example for LVGL C++ SDK
-- This example demonstrates more advanced features

lvgl.print("=== Advanced Lua UI Example ===")

-- Define colors
local COLOR_WHITE = 0xFFFFFF
local COLOR_BLACK = 0x000000
local COLOR_GRAY = 0x333333
local COLOR_BLUE = 0x007AFF
local COLOR_GREEN = 0x34C759
local COLOR_RED = 0xFF3B30
local COLOR_ORANGE = 0xFF9500

-- Create header section
local header_bg = lvgl.create_text("", COLOR_GRAY, root)
header_bg:set_size(800, 60)
header_bg:set_pos(0, 0)
header_bg:set_bg_color(COLOR_GRAY)

local title = lvgl.create_text_ex("Advanced UI Demo", COLOR_WHITE, header_bg,
                                   lvgl.LvText.TextStyle.FontSize28Bold,
                                   lvgl.LvText.FontType.Auto)
title:set_alignment(LV_ALIGN_CENTER, 0, 0)

-- Create content area
local content_y = 80

-- Section 1: Text Controls
lvgl.create_text_ex("Text Controls", COLOR_BLUE, root,
                      lvgl.LvText.TextStyle.FontSize24Bold,
                      lvgl.LvText.FontType.Auto):set_pos(50, content_y)

content_y = content_y + 40

local text1 = lvgl.create_text("Normal Text - FontSize20", COLOR_WHITE, root)
text1:set_pos(50, content_y)

local text2 = lvgl.create_text_ex("Bold Text - FontSize24Bold", COLOR_GREEN, root,
                                    lvgl.LvText.TextStyle.FontSize24Bold,
                                    lvgl.LvText.FontType.Auto)
text2:set_pos(50, content_y + 30)

local text3 = lvgl.create_text_ex("Large Text - FontSize32Bold", COLOR_ORANGE, root,
                                    lvgl.LvText.TextStyle.FontSize32Bold,
                                    lvgl.LvText.FontType.Auto)
text3:set_pos(50, content_y + 70)

content_y = content_y + 120

-- Section 2: Button Styles
lvgl.create_text_ex("Button Styles", COLOR_BLUE, root,
                      lvgl.LvText.TextStyle.FontSize24Bold,
                      lvgl.LvText.FontType.Auto):set_pos(50, content_y)

content_y = content_y + 40

local btn_blue = lvgl.create_button_ex(150, 40, lvgl.PushButton.ColorStyle.Blue,
                                      "Blue Button", root)
btn_blue:set_pos(50, content_y)
btn_blue:set_on_clicked(function()
    lvgl.print("Blue button clicked!")
end)

local btn_dark = lvgl.create_button_ex(150, 40, lvgl.PushButton.ColorStyle.Dark,
                                       "Dark Button", root)
btn_dark:set_pos(220, content_y)
btn_dark:set_on_clicked(function()
    lvgl.print("Dark button clicked!")
end)

local btn_red = lvgl.create_button_ex(150, 40, lvgl.PushButton.ColorStyle.Red,
                                      "Red Button", root)
btn_red:set_pos(390, content_y)
btn_red:set_on_clicked(function()
    lvgl.print("Red button clicked!")
end)

local btn_gray = lvgl.create_button_ex(150, 40, lvgl.PushButton.ColorStyle.Gray,
                                       "Gray Button", root)
btn_gray:set_pos(560, content_y)
btn_gray:set_on_clicked(function()
    lvgl.print("Gray button clicked!")
end)

content_y = content_y + 60

-- Section 3: Interactive Controls
lvgl.create_text_ex("Interactive Controls", COLOR_BLUE, root,
                      lvgl.LvText.TextStyle.FontSize24Bold,
                      lvgl.LvText.FontType.Auto):set_pos(50, content_y)

content_y = content_y + 40

-- Checkbox group
local checkbox1 = lvgl.create_checkbox(30, 30, COLOR_GRAY, COLOR_BLUE, root)
checkbox1:set_pos(50, content_y)
checkbox1:set_on_clicked(function()
    local state = checkbox1:get_check_state()
    lvgl.print("Checkbox 1: " .. tostring(state))
end)

local cb_label1 = lvgl.create_text("Option 1", COLOR_WHITE, root)
cb_label1:set_pos(90, content_y + 5)

local checkbox2 = lvgl.create_checkbox(30, 30, COLOR_GRAY, COLOR_GREEN, root)
checkbox2:set_pos(200, content_y)
checkbox2:set_on_clicked(function()
    local state = checkbox2:get_check_state()
    lvgl.print("Checkbox 2: " .. tostring(state))
end)

local cb_label2 = lvgl.create_text("Option 2", COLOR_WHITE, root)
cb_label2:set_pos(240, content_y + 5)

local checkbox3 = lvgl.create_checkbox(30, 30, COLOR_GRAY, COLOR_RED, root)
checkbox3:set_pos(350, content_y)
checkbox3:set_on_clicked(function()
    local state = checkbox3:get_check_state()
    lvgl.print("Checkbox 3: " .. tostring(state))
end)

local cb_label3 = lvgl.create_text("Option 3", COLOR_WHITE, root)
cb_label3:set_pos(390, content_y + 5)

content_y = content_y + 60

-- Switch group
local switch1 = lvgl.create_switch(64, 32, root)
switch1:set_pos(50, content_y)
switch1:set_on_clicked(function()
    local state = switch1:is_check()
    switch1:set_check(not state)
    lvgl.print("Switch 1: " .. tostring(not state))
end)

local sw_label1 = lvgl.create_text("Switch 1", COLOR_WHITE, root)
sw_label1:set_pos(130, content_y + 5)

local switch2 = lvgl.create_switch(64, 32, root)
switch2:set_pos(250, content_y)
switch2:set_on_clicked(function()
    local state = switch2:is_check()
    switch2:set_check(not state)
    lvgl.print("Switch 2: " .. tostring(not state))
end)

local sw_label2 = lvgl.create_text("Switch 2", COLOR_WHITE, root)
sw_label2:set_pos(330, content_y + 5)

local switch3 = lvgl.create_switch(64, 32, root)
switch3:set_pos(450, content_y)
switch3:set_on_clicked(function()
    local state = switch3:is_check()
    switch3:set_check(not state)
    lvgl.print("Switch 3: " .. tostring(not state))
end)

local sw_label3 = lvgl.create_text("Switch 3", COLOR_WHITE, root)
sw_label3:set_pos(530, content_y + 5)

content_y = content_y + 60

-- Section 4: Action Buttons
lvgl.create_text_ex("Actions", COLOR_BLUE, root,
                      lvgl.LvText.TextStyle.FontSize24Bold,
                      lvgl.LvText.FontType.Auto):set_pos(50, content_y)

content_y = content_y + 40

local btn_reset = lvgl.create_button_ex(120, 40, lvgl.PushButton.ColorStyle.DarkGray,
                                       "Reset All", root)
btn_reset:set_pos(50, content_y)
btn_reset:set_on_clicked(function()
    checkbox1:set_on_clicked(function() end)
    checkbox2:set_on_clicked(function() end)
    checkbox3:set_on_clicked(function() end)
    lvgl.print("All controls reset!")
end)

local btn_toggle = lvgl.create_button_ex(120, 40, lvgl.PushButton.ColorStyle.LightBlue,
                                        "Toggle All", root)
btn_toggle:set_pos(190, content_y)
btn_toggle:set_on_clicked(function()
    local s1 = not switch1:is_check()
    local s2 = not switch2:is_check()
    local s3 = not switch3:is_check()
    switch1:set_check(s1)
    switch2:set_check(s2)
    switch3:set_check(s3)
    lvgl.print("All switches toggled!")
end)

local btn_info = lvgl.create_button_ex(120, 40, lvgl.PushButton.ColorStyle.Blue,
                                       "Show Info", root)
btn_info:set_pos(330, content_y)
btn_info:set_on_clicked(function()
    local cb1 = checkbox1:get_check_state()
    local cb2 = checkbox2:get_check_state()
    local cb3 = checkbox3:get_check_state()
    local sw1 = switch1:is_check()
    local sw2 = switch2:is_check()
    local sw3 = switch3:is_check()
    
    lvgl.print("=== Control States ===")
    lvgl.print("Checkbox 1: " .. tostring(cb1))
    lvgl.print("Checkbox 2: " .. tostring(cb2))
    lvgl.print("Checkbox 3: " .. tostring(cb3))
    lvgl.print("Switch 1: " .. tostring(sw1))
    lvgl.print("Switch 2: " .. tostring(sw2))
    lvgl.print("Switch 3: " .. tostring(sw3))
end)

content_y = content_y + 60

-- Status bar
local status_bg = lvgl.create_text("", COLOR_GRAY, root)
status_bg:set_size(800, 40)
status_bg:set_pos(0, 440)
status_bg:set_bg_color(COLOR_GRAY)

local status_text = lvgl.create_text_ex("Ready - Click controls to interact", COLOR_WHITE, status_bg,
                                        lvgl.LvText.TextStyle.FontSize20,
                                        lvgl.LvText.FontType.Auto)
status_text:set_alignment(LV_ALIGN_CENTER, 0, 0)

-- Update status when any control is clicked
local function update_status(msg)
    status_text:set_text(msg)
end

checkbox1:set_on_clicked(function()
    update_status("Checkbox 1: " .. tostring(checkbox1:get_check_state()))
end)

checkbox2:set_on_clicked(function()
    update_status("Checkbox 2: " .. tostring(checkbox2:get_check_state()))
end)

checkbox3:set_on_clicked(function()
    update_status("Checkbox 3: " .. tostring(checkbox3:get_check_state()))
end)

switch1:set_on_clicked(function()
    local state = switch1:is_check()
    switch1:set_check(not state)
    update_status("Switch 1: " .. tostring(not state))
end)

switch2:set_on_clicked(function()
    local state = switch2:is_check()
    switch2:set_check(not state)
    update_status("Switch 2: " .. tostring(not state))
end)

switch3:set_on_clicked(function()
    local state = switch3:is_check()
    switch3:set_check(not state)
    update_status("Switch 3: " .. tostring(not state))
end)

btn_reset:set_on_clicked(function()
    update_status("All controls reset!")
end)

btn_toggle:set_on_clicked(function()
    update_status("All switches toggled!")
end)

btn_info:set_on_clicked(function()
    update_status("Control states printed to console")
end)

lvgl.print("=== Advanced UI setup completed! ===")
