#pragma once

#include "../core/Object.h"
#include "../core/Types.h"
#include "../signals/Signal.h"
#include "../properties/Property.h"
#include <map>
#include <memory>
#include <functional>

namespace QGL {

// 前向声明
class Widget;
class ThemeManager;

/**
 * @brief 主题类型
 */
enum class ThemeType {
    Light,      // 浅色主题
    Dark,       // 深色主题
    System,     // 系统主题
    Custom      // 自定义主题
};

/**
 * @brief 主题模式
 */
enum class ThemeMode {
    Static,     // 静态主题
    Dynamic,    // 动态主题（跟随系统）
    Scheduled   // 定时主题（根据时间自动切换）
};

/**
 * @brief 主题变量
 * 
 * 主题变量用于定义可配置的主题属性，如颜色、字体、间距等。
 */
class ThemeVariable {
public:
    ThemeVariable() = default;
    ThemeVariable(const String& name, const Variant& value, const String& description = "");
    
    String name() const { return m_name; }
    Variant value() const { return m_value; }
    String description() const { return m_description; }
    
    void setValue(const Variant& value) { m_value = value; }
    void setDescription(const String& description) { m_description = description; }
    
    // 类型转换便利方法
    Color asColor() const;
    int asInt() const;
    double asDouble() const;
    String asString() const;
    Size asSize() const;
    Point asPoint() const;
    Rect asRect() const;

private:
    String m_name;
    Variant m_value;
    String m_description;
};

/**
 * @brief 主题样式
 * 
 * 包含一组样式规则，可以应用到特定的控件或控件状态。
 */
class ThemeStyle : public Object {
    QGL_OBJECT(ThemeStyle)

public:
    explicit ThemeStyle(Object* parent = nullptr);
    explicit ThemeStyle(const String& name, Object* parent = nullptr);
    
    // 样式名称
    void setName(const String& name);
    String name() const;
    
    // 样式属性
    void setProperty(const String& property, const Variant& value);
    Variant property(const String& property) const;
    void removeProperty(const String& property);
    
    // 批量设置
    void setProperties(const std::map<String, Variant>& properties);
    std::map<String, Variant> properties() const;
    
    // 继承
    void setParentStyle(std::shared_ptr<ThemeStyle> parent);
    std::shared_ptr<ThemeStyle> parentStyle() const;
    
    // 状态样式
    void setStateStyle(const String& state, std::shared_ptr<ThemeStyle> style);
    std::shared_ptr<ThemeStyle> stateStyle(const String& state) const;
    void removeStateStyle(const String& state);
    
    // 伪类样式
    void setPseudoStyle(const String& pseudo, std::shared_ptr<ThemeStyle> style);
    std::shared_ptr<ThemeStyle> pseudoStyle(const String& pseudo) const;
    void removePseudoStyle(const String& pseudo);
    
    // 应用样式到控件
    void applyTo(Widget* widget) const;
    void applyStateTo(Widget* widget, const String& state) const;
    
    // 合并样式
    std::shared_ptr<ThemeStyle> merged(std::shared_ptr<ThemeStyle> other) const;
    
    // CSS 导入/导出
    void fromCSS(const String& css);
    String toCSS() const;

private:
    String m_name;
    std::map<String, Variant> m_properties;
    std::shared_ptr<ThemeStyle> m_parentStyle;
    std::map<String, std::shared_ptr<ThemeStyle>> m_stateStyles;
    std::map<String, std::shared_ptr<ThemeStyle>> m_pseudoStyles;
};

/**
 * @brief 主题基类
 * 
 * Theme定义了一套完整的UI主题，包括颜色、字体、样式等。
 */
class Theme : public Object {
    QGL_OBJECT(Theme)

public:
    explicit Theme(Object* parent = nullptr);
    explicit Theme(const String& name, ThemeType type = ThemeType::Custom, Object* parent = nullptr);
    
    // 主题信息
    void setName(const String& name);
    String name() const;
    
    void setDisplayName(const String& displayName);
    String displayName() const;
    
    void setDescription(const String& description);
    String description() const;
    
    void setVersion(const String& version);
    String version() const;
    
    void setAuthor(const String& author);
    String author() const;
    
    void setType(ThemeType type);
    ThemeType type() const;
    
    // 主题变量
    void setVariable(const String& name, const Variant& value, const String& description = "");
    ThemeVariable variable(const String& name) const;
    void removeVariable(const String& name);
    std::map<String, ThemeVariable> variables() const;
    
    // 样式管理
    void setStyle(const String& selector, std::shared_ptr<ThemeStyle> style);
    std::shared_ptr<ThemeStyle> style(const String& selector) const;
    void removeStyle(const String& selector);
    std::map<String, std::shared_ptr<ThemeStyle>> styles() const;
    
    // 基础颜色
    void setPrimaryColor(const Color& color);
    Color primaryColor() const;
    
    void setSecondaryColor(const Color& color);
    Color secondaryColor() const;
    
    void setBackgroundColor(const Color& color);
    Color backgroundColor() const;
    
    void setSurfaceColor(const Color& color);
    Color surfaceColor() const;
    
    void setTextColor(const Color& color);
    Color textColor() const;
    
    void setAccentColor(const Color& color);
    Color accentColor() const;
    
    // 主题资源
    void setIconPath(const String& path);
    String iconPath() const;
    
    void setFontPath(const String& path);
    String fontPath() const;
    
    void setImagePath(const String& path);
    String imagePath() const;
    
    // 主题文件操作
    bool loadFromFile(const String& filePath);
    bool saveToFile(const String& filePath) const;
    
    bool loadFromJSON(const String& json);
    String toJSON() const;
    
    // 应用主题
    void applyToWidget(Widget* widget) const;
    void applyToApplication() const;
    
    // 克隆主题
    std::shared_ptr<Theme> clone() const;

    // 信号
    SIGNAL(variableChanged, const String&);      // 变量改变
    SIGNAL(styleChanged, const String&);         // 样式改变
    SIGNAL(themeChanged);                        // 主题改变

private:
    String m_name;
    String m_displayName;
    String m_description;
    String m_version;
    String m_author;
    ThemeType m_type;
    
    std::map<String, ThemeVariable> m_variables;
    std::map<String, std::shared_ptr<ThemeStyle>> m_styles;
    
    String m_iconPath;
    String m_fontPath;
    String m_imagePath;
};

/**
 * @brief 预定义主题
 */
class LightTheme : public Theme {
    QGL_OBJECT(LightTheme)

public:
    explicit LightTheme(Object* parent = nullptr);
    
private:
    void setupDefaultColors();
    void setupDefaultStyles();
};

class DarkTheme : public Theme {
    QGL_OBJECT(DarkTheme)

public:
    explicit DarkTheme(Object* parent = nullptr);
    
private:
    void setupDefaultColors();
    void setupDefaultStyles();
};

/**
 * @brief 主题管理器
 * 
 * ThemeManager负责管理应用程序的主题系统，包括主题注册、
 * 切换、持久化等功能。
 */
class ThemeManager : public Object {
    QGL_OBJECT(ThemeManager)

public:
    explicit ThemeManager(Object* parent = nullptr);
    ~ThemeManager() override;

    // 单例访问
    static ThemeManager* instance();
    static void setInstance(ThemeManager* manager);

    // 主题注册
    void registerTheme(std::shared_ptr<Theme> theme);
    void unregisterTheme(const String& name);
    bool isThemeRegistered(const String& name) const;
    
    // 主题访问
    std::shared_ptr<Theme> theme(const String& name) const;
    StringList availableThemes() const;
    
    // 当前主题
    void setCurrentTheme(const String& name);
    void setCurrentTheme(std::shared_ptr<Theme> theme);
    std::shared_ptr<Theme> currentTheme() const;
    String currentThemeName() const;
    
    // 主题模式
    void setThemeMode(ThemeMode mode);
    ThemeMode themeMode() const;
    
    // 定时主题切换
    void setScheduledThemes(const String& lightTheme, const String& darkTheme);
    void setLightThemeTime(int hour, int minute);
    void setDarkThemeTime(int hour, int minute);
    
    // 系统主题跟随
    void setFollowSystemTheme(bool follow);
    bool followSystemTheme() const;
    
    // 主题预览
    void previewTheme(const String& name);
    void endPreview();
    bool isPreviewMode() const;
    
    // 主题文件管理
    void setThemeDirectory(const String& directory);
    String themeDirectory() const;
    
    void loadThemesFromDirectory();
    void loadThemeFromFile(const String& filePath);
    
    // 自定义主题
    std::shared_ptr<Theme> createCustomTheme(const String& name, std::shared_ptr<Theme> baseTheme = nullptr);
    void saveCustomTheme(std::shared_ptr<Theme> theme);
    void deleteCustomTheme(const String& name);
    
    // 主题变量
    void setGlobalVariable(const String& name, const Variant& value);
    Variant globalVariable(const String& name) const;
    void removeGlobalVariable(const String& name);
    
    // 样式选择器
    std::shared_ptr<ThemeStyle> selectStyle(Widget* widget) const;
    std::shared_ptr<ThemeStyle> selectStyle(const String& selector) const;
    
    // 动画效果
    void setThemeTransitionEnabled(bool enabled);
    bool isThemeTransitionEnabled() const;
    
    void setTransitionDuration(int duration);
    int transitionDuration() const;
    
    // 缓存管理
    void clearStyleCache();
    void setCacheEnabled(bool enabled);
    bool isCacheEnabled() const;

    // 信号
    SIGNAL(themeChanged, const String&);         // 主题改变
    SIGNAL(themeModeChanged, ThemeMode);         // 主题模式改变
    SIGNAL(previewStarted, const String&);       // 预览开始
    SIGNAL(previewEnded);                        // 预览结束
    SIGNAL(globalVariableChanged, const String&); // 全局变量改变

protected:
    // 内部方法
    void applyTheme(std::shared_ptr<Theme> theme, bool animated = false);
    void updateScheduledTheme();
    void detectSystemTheme();
    
    // 样式计算
    std::shared_ptr<ThemeStyle> computeEffectiveStyle(Widget* widget) const;
    String generateStyleSelector(Widget* widget) const;

private:
    class ThemeManagerPrivate;
    std::unique_ptr<ThemeManagerPrivate> d_ptr;
    
    static ThemeManager* s_instance;
    
    void initializeThemeManager();
    void setupDefaultThemes();
    void setupSystemThemeDetection();
    void setupScheduledThemeSwitch();
};

/**
 * @brief 主题助手宏和函数
 */
#define QGL_THEME() \
    QGL::ThemeManager::instance()->currentTheme()

#define QGL_THEME_VAR(name) \
    QGL::ThemeManager::instance()->currentTheme()->variable(name).value()

#define QGL_THEME_COLOR(name) \
    QGL::ThemeManager::instance()->currentTheme()->variable(name).asColor()

#define QGL_SET_THEME(name) \
    QGL::ThemeManager::instance()->setCurrentTheme(name)

// 便利函数
namespace Theme {
    void applyToWidget(Widget* widget);
    void applyToWidget(Widget* widget, const String& themeName);
    
    Color primaryColor();
    Color secondaryColor();
    Color backgroundColor();
    Color textColor();
    Color accentColor();
    
    void setLightTheme();
    void setDarkTheme();
    void toggleTheme();
}

} // namespace QGL