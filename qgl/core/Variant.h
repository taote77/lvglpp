#ifndef QGL_CORE_VARIANT_H
#define QGL_CORE_VARIANT_H

#include "Types.h"
#include <memory>
#include <type_traits>
#include <stdexcept>

namespace QGL {

// 属性类型枚举
enum class PropertyType {
    Invalid,
    Bool,
    Int,
    Float,
    Double,
    String,
    Color,
    Point,
    Size,
    Rect,
    Custom
};

// 通用值类型包装器
class Variant {
public:
    Variant() : m_type(PropertyType::Invalid) {}
    
    template<typename T>
    Variant(const T& value) : m_type(getTypeId<T>()), m_data(std::make_shared<T>(value)) {}
    
    Variant(const Variant& other) : m_type(other.m_type), m_data(other.m_data) {}
    
    Variant& operator=(const Variant& other) {
        if (this != &other) {
            m_type = other.m_type;
            m_data = other.m_data;
        }
        return *this;
    }
    
    template<typename T>
    T getValue() const {
        if (m_type != getTypeId<T>()) {
            throw std::runtime_error("Type mismatch in Variant::getValue");
        }
        return *static_cast<T*>(m_data.get());
    }
    
    template<typename T>
    void setValue(const T& value) {
        m_type = getTypeId<T>();
        m_data = std::make_shared<T>(value);
    }
    
    PropertyType getType() const { return m_type; }
    bool isValid() const { return m_data != nullptr && m_type != PropertyType::Invalid; }
    bool isNull() const { return !isValid(); }
    
    // 额外的类型转换方法
    uint32_t toUInt() const { return static_cast<uint32_t>(toInt()); }
    int64_t toInt64() const { return static_cast<int64_t>(toInt()); }
    uint64_t toULongLong() const { return static_cast<uint64_t>(toInt64()); }
    
    // 类型转换支持
    String toString() const;
    int toInt() const;
    float toFloat() const;
    double toDouble() const;
    bool toBool() const;
    
    // 便捷构造函数
    static Variant fromString(const String& str) { return Variant(str); }
    static Variant fromInt(int value) { return Variant(value); }
    static Variant fromFloat(float value) { return Variant(value); }
    static Variant fromDouble(double value) { return Variant(value); }
    static Variant fromBool(bool value) { return Variant(value); }
    static Variant fromColor(const Color& color) { return Variant(color); }
    static Variant fromPoint(const Point& point) { return Variant(point); }
    static Variant fromSize(const Size& size) { return Variant(size); }
    static Variant fromRect(const Rect& rect) { return Variant(rect); }
    
private:
    template<typename T>
    PropertyType getTypeId() const {
        if constexpr (std::is_same_v<T, bool>) {
            return PropertyType::Bool;
        } else if constexpr (std::is_same_v<T, int>) {
            return PropertyType::Int;
        } else if constexpr (std::is_same_v<T, float>) {
            return PropertyType::Float;
        } else if constexpr (std::is_same_v<T, double>) {
            return PropertyType::Double;
        } else if constexpr (std::is_same_v<T, String>) {
            return PropertyType::String;
        } else if constexpr (std::is_same_v<T, Color>) {
            return PropertyType::Color;
        } else if constexpr (std::is_same_v<T, Point>) {
            return PropertyType::Point;
        } else if constexpr (std::is_same_v<T, Size>) {
            return PropertyType::Size;
        } else if constexpr (std::is_same_v<T, Rect>) {
            return PropertyType::Rect;
        } else {
            return PropertyType::Custom;
        }
    }
    
    PropertyType m_type;
    std::shared_ptr<void> m_data;
};

} // namespace QGL

#endif // QGL_CORE_VARIANT_H