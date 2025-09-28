#ifndef QGL_PROPERTIES_STYLESHEET_H
#define QGL_PROPERTIES_STYLESHEET_H

#include "../core/Types.h"
#include "../core/Variant.h"
#include <map>
#include <vector>
#include <functional>

namespace QGL {

class Object;

// CSS 选择器类型
enum class SelectorType {
    Universal,    // *
    Type,         // Widget
    Class,        // .className
    Id,           // #objectName
    Attribute,    // [attribute=value]
    PseudoClass   // :hover, :pressed
};

// CSS 选择器
struct Selector {
    SelectorType type;
    String value;
    String attribute;
    String attributeValue;
    
    Selector(SelectorType t = SelectorType::Universal, const String& v = "")
        : type(t), value(v) {}
};

// CSS 样式规则
struct StyleRule {
    std::vector<Selector> selectors;
    std::map<String, String> properties;
};

// CSS 样式表解析器
class StyleSheetParser {
public:
    StyleSheetParser();
    
    // 解析CSS字符串
    std::vector<StyleRule> parse(const String& css);
    
    // 解析单个规则
    StyleRule parseRule(const String& rule);
    
    // 解析选择器
    std::vector<Selector> parseSelectors(const String& selectors);
    Selector parseSelector(const String& selector);
    
    // 解析属性声明
    std::map<String, String> parseDeclarations(const String& declarations);
    
    // 工具方法
    String trim(const String& str);
    std::vector<String> split(const String& str, char delimiter);
    
private:
    // 词法分析状态
    enum class ParseState {
        Selector,
        Declarations,
        Comment
    };
    
    bool isWhitespace(char c);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
};

// CSS 样式应用器
class StyleApplicator {
public:
    StyleApplicator();
    
    // 应用样式表到对象
    void applyStyleSheet(Object* object, const String& css);
    void applyStyleRules(Object* object, const std::vector<StyleRule>& rules);
    
    // 检查选择器是否匹配对象
    bool matchesSelector(Object* object, const Selector& selector);
    bool matchesSelectors(Object* object, const std::vector<Selector>& selectors);
    
    // 应用样式属性
    void applyStyleProperty(Object* object, const String& property, const String& value);
    
    // 解析CSS值
    Variant parseValue(const String& value, PropertyType expectedType = PropertyType::String);
    Color parseColor(const String& value);
    int parseLength(const String& value);
    float parseNumber(const String& value);
    
private:
    StyleSheetParser m_parser;
    
    // 预定义颜色映射
    std::map<String, Color> m_namedColors;
    
    void initializeNamedColors();
};

// CSS 属性映射器
class PropertyMapper {
public:
    PropertyMapper();
    
    // 注册CSS属性到对象属性的映射
    void registerMapping(const String& cssProperty, const String& objectProperty);
    void registerMapping(const String& cssProperty, const String& objectProperty, 
                         std::function<Variant(const String&)> converter);
    
    // 获取映射
    String getObjectProperty(const String& cssProperty);
    std::function<Variant(const String&)> getConverter(const String& cssProperty);
    
    // 预定义映射
    void initializeDefaultMappings();
    
private:
    std::map<String, String> m_propertyMappings;
    std::map<String, std::function<Variant(const String&)>> m_valueConverters;
};

} // namespace QGL

#endif // QGL_PROPERTIES_STYLESHEET_H