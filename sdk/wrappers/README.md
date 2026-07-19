# LVGL SDK 多语言 Wrapper

## 概述

本项目提供了LVGL SDK的多语言包装器，支持Lua语言，便于不同语言的开发者使用LVGL创建UI界面。

## 支持的语言

| 语言 | 绑定库 | 状态 |
|------|--------|------|
| Lua  | sol2   | ✅ 已实现 |

## 目录结构

```
sdk/
├── core/           # 核心功能
├── wrappers/       # 统一包装器目录
│   ├── lua/        # Lua包装器
│   └── CMakeLists.txt
├── lvgl/           # LVGL库
└── net/            # 网络相关
```

## 构建

### 依赖

- **Lua**: Lua 5.4+ 和 sol2
- **CMake**: 3.16+
- **C++ 编译器**: 支持C++17

### 构建步骤

```bash
# 创建构建目录
mkdir -p build
cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译
make -j4
```

## 使用

### Lua 示例

```lua
local lvgl = require("lvgl")

-- 创建根控件
local root = lvgl.create_root()

-- 创建文本
local text = lvgl.create_text(root, "Hello Lua!")
text:set_pos(50, 50)

-- 创建按钮
local button = lvgl.create_button(root, "Click Me")
button:set_pos(50, 100)
button:set_on_clicked(function()
    text:set_text("Button Clicked!")
end)
```

## API 参考

### 基础功能

| 函数 | 描述 |
|------|------|
| `lvgl.create_root()` | 创建根控件 |
| `lvgl.print(msg)` | 打印消息 |
| `lvgl.log(msg)` | 记录日志 |

### 控件创建

| 函数 | 描述 |
|------|------|
| `lvgl.create_text(root, text, color)` | 创建文本控件 |
| `lvgl.create_button(root, width, height, style, text)` | 创建按钮 |
| `lvgl.create_image(root, src_path)` | 创建图片 |

### 控件方法

所有控件都支持以下基础方法：

- `set_pos(x, y)` - 设置位置
- `get_x()`, `get_y()` - 获取位置
- `set_size(width, height)` - 设置大小
- `set_width(width)`, `set_height(height)` - 设置宽度/高度
- `set_bg_color(color)` - 设置背景色
- `set_visible(visible)` - 设置可见性
- `set_enable(enable)` - 设置启用状态

## 示例

### Lua 示例

```bash
cd lua
examples/
```

## 测试

### Lua 测试

```bash
cd build
./target/bin/lua_ui
```

## 开发

### 添加新控件

1. 在 `WidgetLuaBind.h` 和 `WidgetLuaBind.cc` 中添加对应的Lua绑定
2. 保持API风格一致

### 添加新语言

1. 在 `wrappers/` 目录下创建新语言目录
2. 创建对应的绑定代码
3. 更新 `wrappers/CMakeLists.txt` 添加新语言的构建选项

## 许可证

与LVGL SDK相同的许可证。

## 贡献

欢迎提交PR和Issue，帮助改进这个项目。

## 联系

如有问题，请联系项目维护者。
