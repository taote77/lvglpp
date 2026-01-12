-- Lua UI Example for LVGL C++ SDK

lvgl.print("Starting Lua UI Example...")

-- Create title text
local title = lvgl.create_text_ex("LVGL Lua UI Demo", 0xFFFFFF, root,
    lvgl.LvText.TextStyle.FontSize32Bold,
    lvgl.LvText.FontType.Auto)
title:set_pos(50, 30)
title:set_alignment(LV_ALIGN_TOP_MID, 0, 0)

-- Create subtitle text
local subtitle = lvgl.create_text("Build UI with Lua script", 0xCCCCCC, root)
subtitle:set_pos(50, 80)

-- Create a button
local button = lvgl.create_button_ex(200, 50, lvgl.PushButton.ColorStyle.Blue,
    "Click Me", root)
button:set_pos(50, 150)
button:set_on_clicked(function()
    lvgl.print("Button clicked!")
    subtitle:set_text("Button was clicked!")
end)

-- Create another button with different style
local button2 = lvgl.create_button_ex(200, 50, lvgl.PushButton.ColorStyle.Red,
    "Reset", root)
button2:set_pos(300, 150)
button2:set_on_clicked(function()
    lvgl.print("Reset button clicked!")
    subtitle:set_text("Build UI with Lua scripts")
end)

-- Create a checkbox
local checkbox = lvgl.create_checkbox(30, 30, 0x333333, 0x00AAFF, root)
checkbox:set_pos(50, 250)
checkbox:set_on_clicked(function()
    local state = checkbox:get_check_state()
    lvgl.print("Checkbox state: " .. tostring(state))
    if state then
        subtitle:set_text("Checkbox is checked")
    else
        subtitle:set_text("Checkbox is unchecked")
    end
end)

-- Create checkbox label
local checkbox_label = lvgl.create_text("Enable feature", 0xFFFFFF, root)
checkbox_label:set_pos(90, 255)

-- Create a switch
local switch = lvgl.create_switch(64, 32, root)
switch:set_pos(300, 250)
switch:set_on_clicked(function()
    local state = switch:is_check()
    lvgl.print("Switch state: " .. tostring(state))
    if state then
        switch:set_check(false)
        subtitle:set_text("Switch is OFF")
    else
        switch:set_check(true)
        subtitle:set_text("Switch is ON")
    end
end)

-- Create switch label
local switch_label = lvgl.create_text("Toggle Switch", 0xFFFFFF, root)
switch_label:set_pos(380, 255)

-- Create info text
local info_text = lvgl.create_text("Try clicking the controls above!", 0x888888, root)
info_text:set_pos(50, 350)

lvgl.print("Lua UI setup completed!")
