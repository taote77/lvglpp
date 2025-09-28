#ifndef QGL_CORE_TYPES_H
#define QGL_CORE_TYPES_H

#include <string>
#include <vector>
#include <cstdint>

namespace QGL {

// 基础类型定义
using String = std::string;
using StringList = std::vector<String>;

// 颜色类型
class Color {
public:
    Color() = default;
    Color(uint32_t rgba) : m_rgba(rgba) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : m_rgba((r << 24) | (g << 16) | (b << 8) | a) {}
    Color(const String& hexColor);
    
    uint8_t red() const { return (m_rgba >> 24) & 0xFF; }
    uint8_t green() const { return (m_rgba >> 16) & 0xFF; }
    uint8_t blue() const { return (m_rgba >> 8) & 0xFF; }
    uint8_t alpha() const { return m_rgba & 0xFF; }
    
    uint32_t rgba() const { return m_rgba; }
    String toString() const;
    
    // 常用颜色静态成员
    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Transparent;
    
private:
    uint32_t m_rgba = 0x000000FF; // 黑色，不透明
};

// 几何类型
struct Point {
    int x = 0, y = 0;
    Point() = default;
    Point(int x_, int y_) : x(x_), y(y_) {}
    
    Point operator+(const Point& other) const { return Point(x + other.x, y + other.y); }
    Point operator-(const Point& other) const { return Point(x - other.x, y - other.y); }
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

struct Size {
    int width = 0, height = 0;
    Size() = default;
    Size(int w, int h) : width(w), height(h) {}
    
    bool isValid() const { return width >= 0 && height >= 0; }
    bool isEmpty() const { return width <= 0 || height <= 0; }
    Size expandedTo(const Size& other) const {
        return Size(std::max(width, other.width), std::max(height, other.height));
    }
    Size boundedTo(const Size& other) const {
        return Size(std::min(width, other.width), std::min(height, other.height));
    }
};

struct Rect {
    int x = 0, y = 0, width = 0, height = 0;
    Rect() = default;
    Rect(int x_, int y_, int w, int h) : x(x_), y(y_), width(w), height(h) {}
    Rect(const Point& pos, const Size& size) : x(pos.x), y(pos.y), width(size.width), height(size.height) {}
    
    Point topLeft() const { return Point(x, y); }
    Point topRight() const { return Point(x + width, y); }
    Point bottomLeft() const { return Point(x, y + height); }
    Point bottomRight() const { return Point(x + width, y + height); }
    Point center() const { return Point(x + width/2, y + height/2); }
    Size size() const { return Size(width, height); }
    
    bool contains(const Point& point) const {
        return point.x >= x && point.x < x + width && 
               point.y >= y && point.y < y + height;
    }
    
    bool contains(const Rect& rect) const {
        return rect.x >= x && rect.y >= y && 
               rect.x + rect.width <= x + width && 
               rect.y + rect.height <= y + height;
    }
    
    bool intersects(const Rect& rect) const {
        return !(rect.x >= x + width || rect.x + rect.width <= x ||
                rect.y >= y + height || rect.y + rect.height <= y);
    }
    
    bool isValid() const { return width >= 0 && height >= 0; }
    bool isEmpty() const { return width <= 0 || height <= 0; }
};

// 对齐枚举
enum class Alignment {
    Left = 0x01,
    Right = 0x02,
    HCenter = 0x04,
    Top = 0x08,
    Bottom = 0x10,
    VCenter = 0x20,
    Center = HCenter | VCenter
};

// 方向枚举
enum class Orientation {
    Horizontal,
    Vertical
};

// 焦点策略
enum class FocusPolicy {
    NoFocus,
    TabFocus,
    ClickFocus,
    StrongFocus
};

} // namespace QGL

#endif // QGL_CORE_TYPES_H