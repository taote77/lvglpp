#include "Variant.h"
#include <sstream>

namespace QGL {

String Variant::toString() const {
    if (!isValid()) {
        return String();
    }
    
    switch (m_type) {
    case PropertyType::Bool:
        return getValue<bool>() ? "true" : "false";
    case PropertyType::Int:
        return std::to_string(getValue<int>());
    case PropertyType::Float:
        return std::to_string(getValue<float>());
    case PropertyType::Double:
        return std::to_string(getValue<double>());
    case PropertyType::String:
        return getValue<String>();
    case PropertyType::Color:
        return getValue<Color>().toString();
    case PropertyType::Point: {
        const auto& point = getValue<Point>();
        return "Point(" + std::to_string(point.x) + "," + std::to_string(point.y) + ")";
    }
    case PropertyType::Size: {
        const auto& size = getValue<Size>();
        return "Size(" + std::to_string(size.width) + "," + std::to_string(size.height) + ")";
    }
    case PropertyType::Rect: {
        const auto& rect = getValue<Rect>();
        return "Rect(" + std::to_string(rect.x) + "," + std::to_string(rect.y) + "," + 
               std::to_string(rect.width) + "," + std::to_string(rect.height) + ")";
    }
    default:
        return String();
    }
}

int Variant::toInt() const {
    if (!isValid()) {
        return 0;
    }
    
    switch (m_type) {
    case PropertyType::Bool:
        return getValue<bool>() ? 1 : 0;
    case PropertyType::Int:
        return getValue<int>();
    case PropertyType::Float:
        return static_cast<int>(getValue<float>());
    case PropertyType::Double:
        return static_cast<int>(getValue<double>());
    case PropertyType::String:
        try {
            return std::stoi(getValue<String>());
        } catch (...) {
            return 0;
        }
    default:
        return 0;
    }
}

float Variant::toFloat() const {
    if (!isValid()) {
        return 0.0f;
    }
    
    switch (m_type) {
    case PropertyType::Bool:
        return getValue<bool>() ? 1.0f : 0.0f;
    case PropertyType::Int:
        return static_cast<float>(getValue<int>());
    case PropertyType::Float:
        return getValue<float>();
    case PropertyType::Double:
        return static_cast<float>(getValue<double>());
    case PropertyType::String:
        try {
            return std::stof(getValue<String>());
        } catch (...) {
            return 0.0f;
        }
    default:
        return 0.0f;
    }
}

double Variant::toDouble() const {
    if (!isValid()) {
        return 0.0;
    }
    
    switch (m_type) {
    case PropertyType::Bool:
        return getValue<bool>() ? 1.0 : 0.0;
    case PropertyType::Int:
        return static_cast<double>(getValue<int>());
    case PropertyType::Float:
        return static_cast<double>(getValue<float>());
    case PropertyType::Double:
        return getValue<double>();
    case PropertyType::String:
        try {
            return std::stod(getValue<String>());
        } catch (...) {
            return 0.0;
        }
    default:
        return 0.0;
    }
}

bool Variant::toBool() const {
    if (!isValid()) {
        return false;
    }
    
    switch (m_type) {
    case PropertyType::Bool:
        return getValue<bool>();
    case PropertyType::Int:
        return getValue<int>() != 0;
    case PropertyType::Float:
        return getValue<float>() != 0.0f;
    case PropertyType::Double:
        return getValue<double>() != 0.0;
    case PropertyType::String: {
        String str = getValue<String>();
        return str == "true" || str == "1" || str == "yes";
    }
    default:
        return false;
    }
}

} // namespace QGL