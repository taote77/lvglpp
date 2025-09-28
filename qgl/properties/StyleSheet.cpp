#include "StyleSheet.h"
#include "../core/Object.h"
#include <sstream>
#include <algorithm>

namespace QGL {

// StyleSheetParser 实现
StyleSheetParser::StyleSheetParser() {}

std::vector<StyleRule> StyleSheetParser::parse(const String& css) {
    std::vector<StyleRule> rules;
    
    String cleaned = css;
    // 移除注释（简化实现）
    size_t commentStart = cleaned.find("/*");
    while (commentStart != String::npos) {
        size_t commentEnd = cleaned.find("*/", commentStart + 2);
        if (commentEnd != String::npos) {
            cleaned.erase(commentStart, commentEnd - commentStart + 2);
        } else {
            cleaned.erase(commentStart);
            break;
        }
        commentStart = cleaned.find("/*");
    }
    
    // 分割规则
    size_t pos = 0;
    while (pos < cleaned.length()) {
        size_t braceStart = cleaned.find('{', pos);
        if (braceStart == String::npos) break;
        
        size_t braceEnd = cleaned.find('}', braceStart);
        if (braceEnd == String::npos) break;
        
        String selectorPart = cleaned.substr(pos, braceStart - pos);
        String declarationPart = cleaned.substr(braceStart + 1, braceEnd - braceStart - 1);
        
        StyleRule rule;
        rule.selectors = parseSelectors(trim(selectorPart));
        rule.properties = parseDeclarations(trim(declarationPart));
        
        if (!rule.selectors.empty()) {
            rules.push_back(rule);
        }
        
        pos = braceEnd + 1;
    }
    
    return rules;
}

std::vector<Selector> StyleSheetParser::parseSelectors(const String& selectors) {
    std::vector<Selector> result;
    std::vector<String> parts = split(selectors, ',');
    
    for (const String& part : parts) {
        String trimmed = trim(part);
        if (!trimmed.empty()) {
            result.push_back(parseSelector(trimmed));
        }
    }
    
    return result;
}

Selector StyleSheetParser::parseSelector(const String& selector) {
    String trimmed = trim(selector);
    
    if (trimmed.empty()) {
        return Selector(SelectorType::Universal);
    }
    
    if (trimmed[0] == '*') {
        return Selector(SelectorType::Universal);
    } else if (trimmed[0] == '.') {
        return Selector(SelectorType::Class, trimmed.substr(1));
    } else if (trimmed[0] == '#') {
        return Selector(SelectorType::Id, trimmed.substr(1));
    } else if (trimmed[0] == ':') {
        return Selector(SelectorType::PseudoClass, trimmed.substr(1));
    } else {
        return Selector(SelectorType::Type, trimmed);
    }
}

std::map<String, String> StyleSheetParser::parseDeclarations(const String& declarations) {
    std::map<String, String> result;
    std::vector<String> parts = split(declarations, ';');
    
    for (const String& part : parts) {
        String trimmed = trim(part);
        if (trimmed.empty()) continue;
        
        size_t colonPos = trimmed.find(':');
        if (colonPos != String::npos) {
            String property = trim(trimmed.substr(0, colonPos));
            String value = trim(trimmed.substr(colonPos + 1));
            
            if (!property.empty() && !value.empty()) {
                result[property] = value;
            }
        }
    }
    
    return result;
}

String StyleSheetParser::trim(const String& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == String::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<String> StyleSheetParser::split(const String& str, char delimiter) {
    std::vector<String> result;
    std::stringstream ss(str);
    String item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

// StyleApplicator 实现
StyleApplicator::StyleApplicator() {
    initializeNamedColors();
}

void StyleApplicator::applyStyleSheet(Object* object, const String& css) {
    if (!object) return;
    
    std::vector<StyleRule> rules = m_parser.parse(css);
    applyStyleRules(object, rules);
}

void StyleApplicator::applyStyleRules(Object* object, const std::vector<StyleRule>& rules) {
    if (!object) return;
    
    for (const StyleRule& rule : rules) {
        if (matchesSelectors(object, rule.selectors)) {
            for (const auto& prop : rule.properties) {
                applyStyleProperty(object, prop.first, prop.second);
            }
        }
    }
}

bool StyleApplicator::matchesSelectors(Object* object, const std::vector<Selector>& selectors) {
    // 简化实现：任一选择器匹配即可
    for (const Selector& selector : selectors) {
        if (matchesSelector(object, selector)) {
            return true;
        }
    }
    return false;
}

bool StyleApplicator::matchesSelector(Object* object, const Selector& selector) {
    if (!object) return false;
    
    switch (selector.type) {
    case SelectorType::Universal:
        return true;
        
    case SelectorType::Type:
        return object->className() == selector.value;
        
    case SelectorType::Id:
        return object->getObjectName() == selector.value;
        
    case SelectorType::Class:
        // 简化实现：检查对象名是否包含类名
        return object->getObjectName().find(selector.value) != String::npos;
        
    default:
        return false;
    }
}

void StyleApplicator::applyStyleProperty(Object* object, const String& property, const String& value) {
    if (!object) return;
    
    // CSS属性到对象属性的映射
    if (property == "background-color") {
        Color color = parseColor(value);
        object->setProperty("backgroundColor", Variant::fromColor(color));
    } else if (property == "color") {
        Color color = parseColor(value);
        object->setProperty("textColor", Variant::fromColor(color));
    } else if (property == "width") {
        int width = parseLength(value);
        object->setProperty("width", Variant::fromInt(width));
    } else if (property == "height") {
        int height = parseLength(value);
        object->setProperty("height", Variant::fromInt(height));
    } else if (property == "opacity") {
        float opacity = parseNumber(value);
        object->setProperty("opacity", Variant::fromFloat(opacity));
    } else if (property == "border-radius") {
        int radius = parseLength(value);
        object->setProperty("borderRadius", Variant::fromInt(radius));
    } else if (property == "border-width") {
        int width = parseLength(value);
        object->setProperty("borderWidth", Variant::fromInt(width));
    } else if (property == "border-color") {
        Color color = parseColor(value);
        object->setProperty("borderColor", Variant::fromColor(color));
    }
    // 可以添加更多属性映射
}

Color StyleApplicator::parseColor(const String& value) {
    String trimmed = m_parser.trim(value);
    
    // 十六进制颜色
    if (trimmed[0] == '#') {
        return Color(trimmed);
    }
    
    // 命名颜色
    auto it = m_namedColors.find(trimmed);
    if (it != m_namedColors.end()) {
        return it->second;
    }
    
    // RGB函数（简化）
    if (trimmed.substr(0, 4) == "rgb(") {
        // 解析 rgb(r, g, b) 格式
        // 简化实现，返回默认颜色
        return Color::Black;
    }
    
    return Color::Black;
}

int StyleApplicator::parseLength(const String& value) {
    String trimmed = m_parser.trim(value);
    
    // 移除单位（px, em, % 等）
    String numberPart;
    for (char c : trimmed) {
        if (std::isdigit(c) || c == '.' || c == '-') {
            numberPart += c;
        } else {
            break;
        }
    }
    
    if (numberPart.empty()) return 0;
    
    try {
        return std::stoi(numberPart);
    } catch (...) {
        return 0;
    }
}

float StyleApplicator::parseNumber(const String& value) {
    String trimmed = m_parser.trim(value);
    
    try {
        return std::stof(trimmed);
    } catch (...) {
        return 0.0f;
    }
}

void StyleApplicator::initializeNamedColors() {
    m_namedColors["black"] = Color::Black;
    m_namedColors["white"] = Color::White;
    m_namedColors["red"] = Color::Red;
    m_namedColors["green"] = Color::Green;
    m_namedColors["blue"] = Color::Blue;
    m_namedColors["transparent"] = Color::Transparent;
    
    // 可以添加更多命名颜色
    m_namedColors["gray"] = Color(0x808080FF);
    m_namedColors["silver"] = Color(0xC0C0C0FF);
    m_namedColors["yellow"] = Color(0XFFFF00FF);
    m_namedColors["orange"] = Color(0xFFA500FF);
    m_namedColors["purple"] = Color(0x800080FF);
}

} // namespace QGL