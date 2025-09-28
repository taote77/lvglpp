#ifndef QGL_PROPERTIES_PROPERTY_H
#define QGL_PROPERTIES_PROPERTY_H

#include "../core/Types.h"
#include "../core/Variant.h"
#include "../signals/Signal.h"
#include <functional>
#include <map>

namespace QGL {

// 属性标志
enum class PropertyFlags {
    ReadOnly = 0x01,
    WriteOnly = 0x02,
    ReadWrite = ReadOnly | WriteOnly,
    Animatable = 0x04,
    Bindable = 0x08,
    Scriptable = 0x10
};

// 属性验证器类型
using PropertyValidator = std::function<bool(const Variant&)>;

// 属性类
class Property {
public:
    // 构造函数
    template<typename T>
    Property(const String& name, const T& defaultValue, PropertyFlags flags = PropertyFlags::ReadWrite);
    
    Property(const String& name, const Variant& defaultValue, PropertyFlags flags = PropertyFlags::ReadWrite);
    
    // 值设置和获取
    template<typename T>
    void setValue(const T& value);
    
    void setValue(const Variant& value);
    
    template<typename T>
    T getValue() const;
    
    Variant getVariantValue() const;
    
    // 属性信息
    String getName() const { return m_name; }
    PropertyType getType() const { return m_value.getType(); }
    PropertyFlags getFlags() const { return m_flags; }
    
    // 属性状态查询
    bool isReadOnly() const;
    bool isWriteOnly() const;
    bool isAnimatable() const;
    bool isBindable() const;
    bool isScriptable() const;
    
    // 验证器
    void setValidator(PropertyValidator validator);
    PropertyValidator getValidator() const { return m_validator; }
    bool isValid() const;
    bool validate(const Variant& value) const;
    
    // 默认值
    Variant getDefaultValue() const { return m_defaultValue; }
    void resetToDefault();
    
    // 属性绑定
    void bindTo(Property* sourceProperty);
    void unbind();
    bool isBound() const { return m_boundProperty != nullptr; }
    Property* getBoundProperty() const { return m_boundProperty; }
    
    // 信号
    SIGNAL(valueChanged, const Variant&);
    SIGNAL(aboutToChange, const Variant&);
    
    // 属性描述
    void setDescription(const String& description) { m_description = description; }
    String getDescription() const { return m_description; }
    
    void setCategory(const String& category) { m_category = category; }
    String getCategory() const { return m_category; }
    
private:
    String m_name;
    Variant m_value;
    Variant m_defaultValue;
    PropertyFlags m_flags;
    PropertyValidator m_validator;
    Property* m_boundProperty = nullptr;
    String m_description;
    String m_category;
    
    void updateFromBoundProperty();
    bool internalSetValue(const Variant& value, bool fromBinding = false);
};

// 属性信息结构
struct PropertyInfo {
    String name;
    PropertyType type;
    PropertyFlags flags;
    Variant defaultValue;
    String description;
    String category;
    PropertyValidator validator;
};

// 属性注册系统
class PropertyRegistry {
public:
    static PropertyRegistry& instance();
    
    // 注册属性定义
    template<typename T>
    void registerProperty(const String& className, 
                         const String& propertyName,
                         const T& defaultValue,
                         PropertyFlags flags = PropertyFlags::ReadWrite,
                         const String& description = "",
                         const String& category = "",
                         PropertyValidator validator = nullptr);
    
    void registerProperty(const String& className,
                         const PropertyInfo& info);
    
    // 获取属性信息
    PropertyInfo getPropertyInfo(const String& className, const String& propertyName) const;
    std::vector<PropertyInfo> getClassProperties(const String& className) const;
    std::vector<String> getRegisteredClasses() const;
    
    // 属性别名
    void addPropertyAlias(const String& className, const String& alias, const String& realName);
    String resolvePropertyAlias(const String& className, const String& propertyName) const;
    
    // 创建属性实例
    Property* createProperty(const String& className, const String& propertyName) const;
    
private:
    PropertyRegistry() = default;
    
    std::map<String, std::map<String, PropertyInfo>> m_classProperties;
    std::map<String, std::map<String, String>> m_propertyAliases;
};

// 属性声明宏
#define DECLARE_PROPERTY(TYPE, NAME) \
private: \
    Property m_##NAME{#NAME, TYPE{}}; \
public: \
    void set##NAME(const TYPE& value) { m_##NAME.setValue(value); } \
    TYPE get##NAME() const { return m_##NAME.getValue<TYPE>(); } \
    Property& NAME##Property() { return m_##NAME; }

#define DECLARE_PROPERTY_WITH_DEFAULT(TYPE, NAME, DEFAULT) \
private: \
    Property m_##NAME{#NAME, DEFAULT}; \
public: \
    void set##NAME(const TYPE& value) { m_##NAME.setValue(value); } \
    TYPE get##NAME() const { return m_##NAME.getValue<TYPE>(); } \
    Property& NAME##Property() { return m_##NAME; }

// 属性注册宏
#define REGISTER_PROPERTY(CLASS, PROP, TYPE, DEFAULT) \
    PropertyRegistry::instance().registerProperty<TYPE>(#CLASS, #PROP, DEFAULT)

#define REGISTER_PROPERTY_EX(CLASS, PROP, TYPE, DEFAULT, FLAGS, DESC, CAT) \
    PropertyRegistry::instance().registerProperty<TYPE>(#CLASS, #PROP, DEFAULT, FLAGS, DESC, CAT)

// 模板实现
template<typename T>
Property::Property(const String& name, const T& defaultValue, PropertyFlags flags)
    : m_name(name), m_value(defaultValue), m_defaultValue(defaultValue), m_flags(flags) {}

template<typename T>
void Property::setValue(const T& value) {
    setValue(Variant(value));
}

template<typename T>
T Property::getValue() const {
    return m_value.getValue<T>();
}

template<typename T>
void PropertyRegistry::registerProperty(const String& className, 
                                       const String& propertyName,
                                       const T& defaultValue,
                                       PropertyFlags flags,
                                       const String& description,
                                       const String& category,
                                       PropertyValidator validator) {
    PropertyInfo info;
    info.name = propertyName;
    info.type = Variant(defaultValue).getType();
    info.flags = flags;
    info.defaultValue = Variant(defaultValue);
    info.description = description;
    info.category = category;
    info.validator = validator;
    
    registerProperty(className, info);
}

} // namespace QGL

#endif // QGL_PROPERTIES_PROPERTY_H