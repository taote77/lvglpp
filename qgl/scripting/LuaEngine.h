#pragma once

#include "../core/Object.h"
#include "../core/Types.h"
#include "../signals/Signal.h"
#include <memory>
#include <functional>
#include <map>

// 前向声明Lua结构
struct lua_State;

namespace QGL {

// 前向声明
class Widget;
class LuaEngine;

/**
 * @brief Lua值类型
 */
enum class LuaValueType {
    Nil,
    Boolean,
    Number,
    String,
    Table,
    Function,
    UserData,
    Thread
};

/**
 * @brief Lua值包装器
 * 
 * LuaValue提供C++和Lua之间的类型转换。
 */
class LuaValue {
public:
    LuaValue();
    LuaValue(bool value);
    LuaValue(int value);
    LuaValue(double value);
    LuaValue(const String& value);
    LuaValue(const char* value);
    
    // 类型检查
    LuaValueType type() const;
    bool isNil() const;
    bool isBool() const;
    bool isNumber() const;
    bool isString() const;
    bool isTable() const;
    bool isFunction() const;
    
    // 类型转换
    bool toBool() const;
    int toInt() const;
    double toDouble() const;
    String toString() const;
    
    // 表操作（简化接口）
    void setTableValue(const String& key, const LuaValue& value);
    LuaValue getTableValue(const String& key) const;
    
    // 与Variant的转换
    static LuaValue fromVariant(const Variant& variant);
    Variant toVariant() const;

private:
    LuaValueType m_type;
    union {
        bool boolValue;
        double numberValue;
    };
    String m_stringValue;
    std::map<String, LuaValue> m_tableValue;
};

/**
 * @brief Lua函数包装器
 */
class LuaFunction {
public:
    LuaFunction();
    LuaFunction(lua_State* L, int ref);
    ~LuaFunction();
    
    // 拷贝和移动
    LuaFunction(const LuaFunction& other);
    LuaFunction& operator=(const LuaFunction& other);
    LuaFunction(LuaFunction&& other) noexcept;
    LuaFunction& operator=(LuaFunction&& other) noexcept;
    
    // 函数调用
    LuaValue call() const;
    LuaValue call(const LuaValue& arg1) const;
    LuaValue call(const LuaValue& arg1, const LuaValue& arg2) const;
    LuaValue call(const std::vector<LuaValue>& args) const;
    
    // 有效性检查
    bool isValid() const;

private:
    lua_State* m_L;
    int m_ref;
    
    void addRef();
    void release();
};

/**
 * @brief Lua绑定基类
 * 
 * LuaBind用于将C++对象绑定到Lua环境。
 */
class LuaBind {
public:
    virtual ~LuaBind() = default;
    
    // 绑定到Lua状态
    virtual void bindToLua(lua_State* L) = 0;
    
    // 从Lua获取对象
    template<typename T>
    static T* checkUserData(lua_State* L, int index, const String& typeName);
    
    // 推送对象到Lua
    template<typename T>
    static void pushUserData(lua_State* L, T* obj, const String& typeName);
};

/**
 * @brief Lua脚本引擎
 * 
 * LuaEngine提供完整的Lua脚本执行环境，支持控件绑定、
 * 事件处理、动态界面创建等功能。
 */
class LuaEngine : public Object {
    QGL_OBJECT(LuaEngine)

public:
    explicit LuaEngine(Object* parent = nullptr);
    ~LuaEngine() override;

    // 单例访问
    static LuaEngine* instance();
    static void setInstance(LuaEngine* engine);

    // 脚本执行
    bool executeScript(const String& script);
    bool executeFile(const String& fileName);
    
    // 表达式求值
    LuaValue evaluate(const String& expression);
    
    // 变量操作
    void setGlobal(const String& name, const LuaValue& value);
    LuaValue getGlobal(const String& name) const;
    void removeGlobal(const String& name);
    
    // 函数注册
    void registerFunction(const String& name, std::function<LuaValue(const std::vector<LuaValue>&)> function);
    void registerCFunction(const String& name, int (*function)(lua_State*));
    
    // 类型绑定
    void bindWidget(Widget* widget, const String& name = "");
    void unbindWidget(Widget* widget);
    void unbindAll();
    
    // 模块系统
    void loadModule(const String& moduleName);
    void registerModule(const String& name, std::function<void(lua_State*)> moduleFunction);
    
    // 脚本路径管理
    void addScriptPath(const String& path);
    void removeScriptPath(const String& path);
    void clearScriptPaths();
    StringList scriptPaths() const;
    
    // 包管理
    void setPackagePath(const String& path);
    String packagePath() const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const String&)> handler);
    String lastError() const;
    
    // 调试支持
    void setDebugEnabled(bool enabled);
    bool isDebugEnabled() const;
    
    void enableProfiler(bool enabled);
    bool isProfilerEnabled() const;
    
    // 垃圾收集
    void collectGarbage();
    void setGCThreshold(int threshold);
    int getGCThreshold() const;
    
    // Lua状态访问（高级用法）
    lua_State* luaState() const;
    
    // 安全性
    void setSandboxEnabled(bool enabled);
    bool isSandboxEnabled() const;
    
    void setRestrictedMode(bool enabled);
    bool isRestrictedMode() const;

    // 信号
    SIGNAL(scriptExecuted, const String&);           // 脚本执行完成
    SIGNAL(scriptError, const String&, const String&); // 脚本执行错误
    SIGNAL(functionCalled, const String&);           // 函数被调用
    SIGNAL(variableChanged, const String&);          // 变量改变

protected:
    // 内部方法
    void initializeLua();
    void setupStandardLibraries();
    void setupQGLBindings();
    void setupSandbox();
    
    // 错误处理
    void handleError(const String& error);
    static int luaErrorHandler(lua_State* L);
    
    // 内置函数
    static int luaPrint(lua_State* L);
    static int luaLog(lua_State* L);
    static int luaCreateWidget(lua_State* L);
    static int luaFindWidget(lua_State* L);
    static int luaConnectSignal(lua_State* L);

private:
    class LuaEnginePrivate;
    std::unique_ptr<LuaEnginePrivate> d_ptr;
    
    static LuaEngine* s_instance;
};

/**
 * @brief Widget的Lua绑定
 */
class WidgetLuaBind : public LuaBind {
public:
    void bindToLua(lua_State* L) override;
    
private:
    // Widget方法的Lua包装
    static int lua_Widget_setProperty(lua_State* L);
    static int lua_Widget_getProperty(lua_State* L);
    static int lua_Widget_setPosition(lua_State* L);
    static int lua_Widget_getPosition(lua_State* L);
    static int lua_Widget_setSize(lua_State* L);
    static int lua_Widget_getSize(lua_State* L);
    static int lua_Widget_setVisible(lua_State* L);
    static int lua_Widget_isVisible(lua_State* L);
    static int lua_Widget_setEnabled(lua_State* L);
    static int lua_Widget_isEnabled(lua_State* L);
    
    // 元表设置
    static void setupWidgetMetatable(lua_State* L);
};

/**
 * @brief Lua脚本管理器
 * 
 * LuaScriptManager提供脚本文件管理、热重载等功能。
 */
class LuaScriptManager : public Object {
    QGL_OBJECT(LuaScriptManager)

public:
    explicit LuaScriptManager(Object* parent = nullptr);
    ~LuaScriptManager() override;

    // 脚本文件管理
    bool loadScript(const String& fileName, const String& alias = "");
    bool reloadScript(const String& alias);
    void unloadScript(const String& alias);
    void unloadAllScripts();
    
    // 脚本查找
    bool hasScript(const String& alias) const;
    StringList loadedScripts() const;
    
    // 热重载
    void setHotReloadEnabled(bool enabled);
    bool isHotReloadEnabled() const;
    
    void setWatchDirectory(const String& directory);
    String watchDirectory() const;
    
    // 脚本模板
    void registerTemplate(const String& name, const String& templateContent);
    String getTemplate(const String& name) const;
    bool createScriptFromTemplate(const String& templateName, const String& fileName, 
                                 const std::map<String, String>& variables = {});
    
    // 依赖管理
    void addDependency(const String& script, const String& dependency);
    void removeDependency(const String& script, const String& dependency);
    StringList getDependencies(const String& script) const;

    // 信号
    SIGNAL(scriptLoaded, const String&);             // 脚本加载
    SIGNAL(scriptUnloaded, const String&);           // 脚本卸载
    SIGNAL(scriptReloaded, const String&);           // 脚本重载
    SIGNAL(scriptChanged, const String&);            // 脚本文件改变

private:
    class LuaScriptManagerPrivate;
    std::unique_ptr<LuaScriptManagerPrivate> d_ptr;
    
    void watchFileChanges();
    void processFileChange(const String& fileName);
};

/**
 * @brief Lua便利宏和函数
 */
#define QGL_LUA_EXEC(script) \
    QGL::LuaEngine::instance()->executeScript(script)

#define QGL_LUA_EVAL(expression) \
    QGL::LuaEngine::instance()->evaluate(expression)

#define QGL_LUA_SET_GLOBAL(name, value) \
    QGL::LuaEngine::instance()->setGlobal(name, QGL::LuaValue::fromVariant(value))

#define QGL_LUA_GET_GLOBAL(name) \
    QGL::LuaEngine::instance()->getGlobal(name).toVariant()

#define QGL_LUA_BIND_WIDGET(widget, name) \
    QGL::LuaEngine::instance()->bindWidget(widget, name)

// 便利函数
namespace Lua {
    // 快速执行
    bool exec(const String& script);
    LuaValue eval(const String& expression);
    
    // 变量操作
    void setVar(const String& name, const Variant& value);
    Variant getVar(const String& name);
    
    // 控件绑定
    void bindWidget(Widget* widget, const String& name = "");
    void unbindWidget(Widget* widget);
    
    // 脚本文件
    bool loadFile(const String& fileName);
    bool reloadFile(const String& fileName);
    
    // 模块加载
    void loadModule(const String& moduleName);
    
    // 实用工具
    String escape(const String& str);
    String formatScript(const String& script);
    bool validateSyntax(const String& script);
}

} // namespace QGL