#include "Property.h"
#include <algorithm>

namespace QGL {

// Property 实现
Property::Property(const String& name, const Variant& defaultValue, PropertyFlags flags)
    : m_name(name), m_value(defaultValue), m_defaultValue(defaultValue), m_flags(flags) {}

void Property::setValue(const Variant& value) {
    internalSetValue(value, false);
}

Variant Property::getVariantValue() const {
    return m_value;
}

bool Property::isReadOnly() const {
    return static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::ReadOnly) &&
           !(static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::WriteOnly));
}

bool Property::isWriteOnly() const {
    return static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::WriteOnly) &&
           !(static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::ReadOnly));
}

bool Property::isAnimatable() const {
    return static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::Animatable);
}

bool Property::isBindable() const {
    return static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::Bindable);
}

bool Property::isScriptable() const {
    return static_cast<int>(m_flags) & static_cast<int>(PropertyFlags::Scriptable);
}

void Property::setValidator(PropertyValidator validator) {
    m_validator = validator;
}

bool Property::isValid() const {
    return validate(m_value);
}

bool Property::validate(const Variant& value) const {
    if (m_validator) {
        return m_validator(value);
    }
    return value.isValid();
}

void Property::resetToDefault() {
    setValue(m_defaultValue);
}

void Property::bindTo(Property* sourceProperty) {
    if (!sourceProperty || !isBindable()) {
        return;
    }
    
    // 断开现有绑定
    unbind();
    
    m_boundProperty = sourceProperty;
    
    // 立即同步值
    updateFromBoundProperty();
    
    // 连接信号以保持同步
    if (m_boundProperty) {
        m_boundProperty->valueChanged.connect([this](const Variant&) {
            updateFromBoundProperty();
        });
    }
}

void Property::unbind() {
    if (m_boundProperty) {
        // 简化实现：不使用信号断开，直接清空绑定
        // m_boundProperty->valueChanged.disconnect(this);
        m_boundProperty = nullptr;
    }
}

void Property::updateFromBoundProperty() {
    if (m_boundProperty) {
        internalSetValue(m_boundProperty->getVariantValue(), true);
    }
}

bool Property::internalSetValue(const Variant& value, bool fromBinding) {
    // 检查只读属性
    if (!fromBinding && isReadOnly()) {
        return false;
    }
    
    // 检查值是否有变化
    if (m_value.toString() == value.toString() && m_value.getType() == value.getType()) {
        return true;
    }
    
    // 验证新值
    if (!validate(value)) {
        return false;
    }
    
    // 发送即将变化信号
    aboutToChange.emit(value);
    
    // 设置新值
    Variant oldValue = m_value;
    m_value = value;
    
    // 发送变化信号
    valueChanged.emit(m_value);
    
    return true;
}

// PropertyRegistry 实现
PropertyRegistry& PropertyRegistry::instance() {
    static PropertyRegistry registry;
    return registry;
}

void PropertyRegistry::registerProperty(const String& className,
                                       const PropertyInfo& info) {
    m_classProperties[className][info.name] = info;
}

PropertyInfo PropertyRegistry::getPropertyInfo(const String& className, const String& propertyName) const {
    // 解析别名
    String resolvedName = resolvePropertyAlias(className, propertyName);
    
    auto classIt = m_classProperties.find(className);
    if (classIt != m_classProperties.end()) {
        auto propIt = classIt->second.find(resolvedName);
        if (propIt != classIt->second.end()) {
            return propIt->second;
        }
    }
    
    // 返回空的属性信息
    PropertyInfo empty;
    empty.name = propertyName;
    empty.type = PropertyType::Invalid;
    empty.flags = PropertyFlags::ReadWrite;
    return empty;
}

std::vector<PropertyInfo> PropertyRegistry::getClassProperties(const String& className) const {
    std::vector<PropertyInfo> properties;
    
    auto classIt = m_classProperties.find(className);
    if (classIt != m_classProperties.end()) {
        for (const auto& prop : classIt->second) {
            properties.push_back(prop.second);
        }
    }
    
    return properties;
}

std::vector<String> PropertyRegistry::getRegisteredClasses() const {
    std::vector<String> classes;
    
    for (const auto& classPair : m_classProperties) {
        classes.push_back(classPair.first);
    }
    
    return classes;
}

void PropertyRegistry::addPropertyAlias(const String& className, const String& alias, const String& realName) {
    m_propertyAliases[className][alias] = realName;
}

String PropertyRegistry::resolvePropertyAlias(const String& className, const String& propertyName) const {
    auto classIt = m_propertyAliases.find(className);
    if (classIt != m_propertyAliases.end()) {
        auto aliasIt = classIt->second.find(propertyName);
        if (aliasIt != classIt->second.end()) {
            return aliasIt->second;
        }
    }
    
    return propertyName;
}

Property* PropertyRegistry::createProperty(const String& className, const String& propertyName) const {
    PropertyInfo info = getPropertyInfo(className, propertyName);
    
    if (info.type == PropertyType::Invalid) {
        return nullptr;
    }
    
    Property* property = new Property(info.name, info.defaultValue, info.flags);
    property->setDescription(info.description);
    property->setCategory(info.category);
    property->setValidator(info.validator);
    
    return property;
}

} // namespace QGL