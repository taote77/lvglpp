#include "Types.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace QGL {

// 颜色常量定义
const Color Color::White(0xFFFFFFFF);
const Color Color::Black(0x000000FF);
const Color Color::Red(0xFF0000FF);
const Color Color::Green(0x00FF00FF);
const Color Color::Blue(0x0000FFFF);
const Color Color::Transparent(0x00000000);

Color::Color(const String& hexColor) {
    if (hexColor.empty()) {
        m_rgba = 0x000000FF;
        return;
    }
    
    String color = hexColor;
    if (color[0] == '#') {
        color = color.substr(1);
    }
    
    try {
        if (color.length() == 6) {
            // RGB format
            uint32_t rgb = std::stoul(color, nullptr, 16);
            m_rgba = (rgb << 8) | 0xFF; // Add alpha
        } else if (color.length() == 8) {
            // RGBA format
            m_rgba = std::stoul(color, nullptr, 16);
        } else if (color.length() == 3) {
            // Short RGB format (#FFF -> #FFFFFF)
            uint32_t r = std::stoul(color.substr(0, 1), nullptr, 16) * 17;
            uint32_t g = std::stoul(color.substr(1, 1), nullptr, 16) * 17;
            uint32_t b = std::stoul(color.substr(2, 1), nullptr, 16) * 17;
            m_rgba = (r << 24) | (g << 16) | (b << 8) | 0xFF;
        } else {
            throw std::invalid_argument("Invalid color format");
        }
    } catch (const std::exception&) {
        // 解析失败，使用默认颜色
        m_rgba = 0x000000FF;
    }
}

String Color::toString() const {
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << m_rgba;
    return ss.str();
}

} // namespace QGL