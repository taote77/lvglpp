#ifndef QGL_CORE_OBJECT_H
#define QGL_CORE_OBJECT_H

#include "Types.h"
#include "Variant.h"
#include "../signals/Signal.h"
#include <memory>
#include <map>
#include <string>

namespace QGL {

// 前向声明
class Property;

// 对象基类
class Object : public std::enable_shared_from_this<Object> {
public:
    Object(Object* parent = nullptr);
    virtual ~Object();
    
    // 禁用拷贝，允许移动
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = default;
    Object& operator=(Object&&) = default;
    
    // 对象名称
    void setObjectName(const String& name) { m_objectName = name; }
    String getObjectName() const { return m_objectName; }
    
    // 父子关系管理
    Object* parent() const { return m_parent; }
    void setParent(Object* parent);
    
    const std::vector<Object*>& children() const { return m_children; }
    void addChild(Object* child);
    void removeChild(Object* child);
    
    // 查找子对象
    Object* findChild(const String& name) const;
    template<typename T>
    T* findChild(const String& name = String()) const;
    
    // 属性系统
    void setProperty(const String& name, const Variant& value);
    Variant getProperty(const String& name) const;
    bool hasProperty(const String& name) const;
    
    // 批量属性操作
    void setProperties(const std::map<String, Variant>& properties);
    std::map<String, Variant> getProperties() const;
    
    // 属性变化信号
    SIGNAL(propertyChanged, const String&, const Variant&);
    
    // 对象销毁信号
    SIGNAL(destroyed);
    
    // 类型信息
    virtual String className() const { return "Object"; }
    
    // 类型检查
    template<typename T>
    bool inherits() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }
    
protected:
    // 子类可重写的事件处理函数
    virtual void onPropertyChanged(const String& name, const Variant& value) {}
    virtual void onChildAdded(Object* child) {}
    virtual void onChildRemoved(Object* child) {}
    virtual void onParentChanged(Object* oldParent, Object* newParent) {}
    
private:
    String m_objectName;
    Object* m_parent = nullptr;
    std::vector<Object*> m_children;
    std::map<String, Variant> m_properties;
    
    void removeFromParent();
};

// 模板实现
template<typename T>
T* Object::findChild(const String& name) const {
    for (Object* child : m_children) {
        if (auto typedChild = dynamic_cast<T*>(child)) {
            if (name.empty() || child->getObjectName() == name) {
                return typedChild;
            }
        }
        
        // 递归查找
        if (auto found = child->findChild<T>(name)) {
            return found;
        }
    }
    return nullptr;
}

// 便捷宏定义
#define QGL_OBJECT(ClassName) \
public: \
    String className() const override { return #ClassName; } \
private:

} // namespace QGL

#endif // QGL_CORE_OBJECT_H