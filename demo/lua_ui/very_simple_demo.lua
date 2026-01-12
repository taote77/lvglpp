-- 超级简单的 Lua UI 示例
-- 每一行都有详细注释

-- 第1步：打印一条消息
lvgl.print("开始创建 UI...")

-- 第2步：创建一个文本控件
-- lvgl.create_text(文本内容, 颜色, 父控件)
-- 0xFFFFFF = 白色
-- root = 屏幕的根容器（由 C++ 代码创建）
local my_text = lvgl.create_text("你好 Lua!", 0xFFFFFF, root)

-- 第3步：设置文本的位置
-- set_pos(x坐标, y坐标)
-- 屏幕左上角是 (0, 0)
my_text:set_pos(50, 50)

-- 第4步：创建一个按钮
-- lvgl.create_button(宽度, 高度, 按钮文字, 父控件)
local my_button = lvgl.create_button(200, 60, "点击我", root)

-- 第5步：设置按钮的位置
my_button:set_pos(50, 150)

-- 第6步：给按钮添加点击事件
-- 当按钮被点击时，执行这个函数
my_button:set_on_clicked(function()
    -- 更新文本内容
    my_text:set_text("你点击了按钮！")
    
    -- 打印一条消息到控制台
    lvgl.print("按钮被点击了！")
end)

-- 第7步：创建另一个文本作为状态显示
local status_text = lvgl.create_text("等待点击...", 0x888888, root)
status_text:set_pos(50, 250)

-- 第8步：创建第二个按钮
local button2 = lvgl.create_button(200, 60, "重置", root)
button2:set_pos(300, 150)

-- 第9步：给第二个按钮添加点击事件
button2:set_on_clicked(function()
    -- 重置文本内容
    my_text:set_text("你好 Lua!")
    status_text:set_text("等待点击...")
    
    lvgl.print("已重置")
end)

-- 完成！
lvgl.print("UI 创建完成！")
