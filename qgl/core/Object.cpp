#include "Object.h"
#include <algorithm>

namespace QGL {

Object::Object(Object* parent) : m_parent(parent) {
    if (m_parent) {
        m_parent->addChild(this);
    }
}

Object::~Object() {
    // 发送销毁信号
    destroyed.emit();
    
    // 删除所有子对象
    for (Object* child : m_children) {
        child->m_parent = nullptr;
        delete child;
    }
    m_children.clear();
    
    // 从父对象中移除
    removeFromParent();
}

void Object::setParent(Object* parent) {
    if (m_parent == parent) {
        return;
    }
    
    Object* oldParent = m_parent;
    
    // 从旧父对象中移除
    removeFromParent();
    
    // 设置新父对象
    m_parent = parent;
    if (m_parent) {
        m_parent->addChild(this);
    }
    
    // 调用虚函数通知子类
    onParentChanged(oldParent, parent);
}

void Object::addChild(Object* child) {
    if (!child || std::find(m_children.begin(), m_children.end(), child) != m_children.end()) {
        return;
    }
    
    m_children.push_back(child);
    if (child->m_parent != this) {
        child->setParent(this);
    }
    
    onChildAdded(child);
}

void Object::removeChild(Object* child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        m_children.erase(it);
        if (child->m_parent == this) {
            child->m_parent = nullptr;
        }
        onChildRemoved(child);
    }
}

Object* Object::findChild(const String& name) const {
    for (Object* child : m_children) {
        if (child->getObjectName() == name) {
            return child;
        }
        
        // 递归查找
        if (Object* found = child->findChild(name)) {
            return found;
        }
    }
    return nullptr;
}

void Object::setProperty(const String& name, const Variant& value) {
    Variant oldValue = getProperty(name);
    m_properties[name] = value;
    
    // 如果值确实发生了变化，发送信号
    if (oldValue.toString() != value.toString()) {
        propertyChanged.emit(name, value);
        onPropertyChanged(name, value);
    }
}

Variant Object::getProperty(const String& name) const {
    auto it = m_properties.find(name);
    return (it != m_properties.end()) ? it->second : Variant();
}

bool Object::hasProperty(const String& name) const {
    return m_properties.find(name) != m_properties.end();
}

void Object::setProperties(const std::map<String, Variant>& properties) {
    for (const auto& pair : properties) {
        setProperty(pair.first, pair.second);
    }
}

std::map<String, Variant> Object::getProperties() const {
    return m_properties;
}

void Object::removeFromParent() {
    if (m_parent) {
        auto it = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
        if (it != m_parent->m_children.end()) {
            m_parent->m_children.erase(it);
        }
    }
}

} // namespace QGL