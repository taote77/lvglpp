-- 测试脚本 - 验证控件是否正确创建

lvgl.print("=== 开始测试 ===")

-- 创建一个大的红色背景测试
local test_bg = lvgl.create_text("", 0xFF0000, root)
test_bg:set_size(800, 480)
test_bg:set_pos(0, 0)
test_bg:set_bg_color(0xFF0000)

-- 创建一个大的白色文本
local big_text = lvgl.create_text("TEST", 0xFFFFFF, test_bg)
big_text:set_pos(350, 220)
big_text:set_size(100, 40)
big_text:set_bg_color(0x00FF00)

lvgl.print("=== 测试完成 ===")
