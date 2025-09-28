#include "ThemeManager.h"
#include "../widgets/Widget.h"
#include "../core/Application.h"
#include "../timer/Timer.h"
#include <lvgl.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace QGL {

// ThemeManager静态实例
ThemeManager* ThemeManager::s_instance = nullptr;

// ThemeVariable实现
ThemeVariable::ThemeVariable(const String& name, const Variant& value, const String& description)
    : m_name(name), m_value(value), m_description(description) {
}

Color ThemeVariable::asColor() const {
    return Color(m_value.toInt());
}

int ThemeVariable::asInt() const {
    return m_value.toInt();
}

double ThemeVariable::asDouble() const {
    return m_value.toDouble();
}

String ThemeVariable::asString() const {
    return m_value.toString();
}

Size ThemeVariable::asSize() const {
    return Size(100, 100);
}

Point ThemeVariable::asPoint() const {
    return Point(0, 0);
}

Rect ThemeVariable::asRect() const {
    return Rect(0, 0, 100, 100);
}

// ThemeStyle实现
ThemeStyle::ThemeStyle(Object* parent)
    : Object(parent) {
    setObjectName("ThemeStyle");
}

ThemeStyle::ThemeStyle(const String& name, Object* parent)
    : Object(parent), m_name(name) {
    setObjectName("ThemeStyle");
}

void ThemeStyle::setName(const String& name) {
    m_name = name;
}

String ThemeStyle::name() const {
    return m_name;
}

void ThemeStyle::setProperty(const String& property, const Variant& value) {
    m_properties[property] = value;
}

Variant ThemeStyle::property(const String& property) const {
    auto it = m_properties.find(property);
    if (it != m_properties.end()) {
        return it->second;
    }
    
    if (m_parentStyle) {
        return m_parentStyle->property(property);
    }
    
    return Variant();
}

void ThemeStyle::removeProperty(const String& property) {
    m_properties.erase(property);
}

void ThemeStyle::setProperties(const std::map<String, Variant>& properties) {
    m_properties = properties;
}

std::map<String, Variant> ThemeStyle::properties() const {
    std::map<String, Variant> result = m_properties;
    
    if (m_parentStyle) {
        auto parentProps = m_parentStyle->properties();
        for (const auto& prop : parentProps) {
            if (result.find(prop.first) == result.end()) {
                result[prop.first] = prop.second;
            }
        }
    }
    
    return result;
}

void ThemeStyle::setParentStyle(std::shared_ptr<ThemeStyle> parent) {
    m_parentStyle = parent;
}

std::shared_ptr<ThemeStyle> ThemeStyle::parentStyle() const {
    return m_parentStyle;
}

void ThemeStyle::setStateStyle(const String& state, std::shared_ptr<ThemeStyle> style) {
    m_stateStyles[state] = style;
}

std::shared_ptr<ThemeStyle> ThemeStyle::stateStyle(const String& state) const {
    auto it = m_stateStyles.find(state);
    return it != m_stateStyles.end() ? it->second : nullptr;
}

void ThemeStyle::removeStateStyle(const String& state) {
    m_stateStyles.erase(state);
}

void ThemeStyle::setPseudoStyle(const String& pseudo, std::shared_ptr<ThemeStyle> style) {
    m_pseudoStyles[pseudo] = style;
}

std::shared_ptr<ThemeStyle> ThemeStyle::pseudoStyle(const String& pseudo) const {
    auto it = m_pseudoStyles.find(pseudo);
    return it != m_pseudoStyles.end() ? it->second : nullptr;
}

void ThemeStyle::removePseudoStyle(const String& pseudo) {
    m_pseudoStyles.erase(pseudo);
}

void ThemeStyle::applyTo(Widget* widget) const {
    if (!widget) return;
    
    auto props = properties();
    for (const auto& prop : props) {
        widget->setProperty(prop.first, prop.second);
    }
}

void ThemeStyle::applyStateTo(Widget* widget, const String& state) const {
    if (!widget) return;
    
    auto stateStyle = this->stateStyle(state);
    if (stateStyle) {
        stateStyle->applyTo(widget);
    }
}

std::shared_ptr<ThemeStyle> ThemeStyle::merged(std::shared_ptr<ThemeStyle> other) const {
    auto result = std::make_shared<ThemeStyle>();
    
    auto thisProps = properties();
    auto otherProps = other ? other->properties() : std::map<String, Variant>();
    
    for (const auto& prop : thisProps) {
        result->setProperty(prop.first, prop.second);
    }
    
    for (const auto& prop : otherProps) {
        result->setProperty(prop.first, prop.second);
    }
    
    return result;
}

void ThemeStyle::fromCSS(const String& css) {
    // 简化的CSS解析实现
}

String ThemeStyle::toCSS() const {
    std::ostringstream css;
    css << "." << m_name << " {\n";
    
    for (const auto& prop : m_properties) {
        css << "  " << prop.first << ": " << prop.second.toString() << ";\n";
    }
    
    css << "}\n";
    return css.str();
}

// Theme实现
Theme::Theme(Object* parent)
    : Object(parent), m_type(ThemeType::Custom) {
    setObjectName("Theme");
}

Theme::Theme(const String& name, ThemeType type, Object* parent)
    : Object(parent), m_name(name), m_type(type) {
    setObjectName("Theme");
}

void Theme::setName(const String& name) {
    m_name = name;
}

String Theme::name() const {
    return m_name;
}

void Theme::setDisplayName(const String& displayName) {
    m_displayName = displayName;
}

String Theme::displayName() const {
    return m_displayName.empty() ? m_name : m_displayName;
}

void Theme::setDescription(const String& description) {
    m_description = description;
}

String Theme::description() const {
    return m_description;
}

void Theme::setVersion(const String& version) {
    m_version = version;
}

String Theme::version() const {
    return m_version;
}

void Theme::setAuthor(const String& author) {
    m_author = author;
}

String Theme::author() const {
    return m_author;
}

void Theme::setType(ThemeType type) {
    m_type = type;
}

ThemeType Theme::type() const {
    return m_type;
}

void Theme::setVariable(const String& name, const Variant& value, const String& description) {
    m_variables[name] = ThemeVariable(name, value, description);
    variableChanged.emit(name);
}

ThemeVariable Theme::variable(const String& name) const {
    auto it = m_variables.find(name);
    return it != m_variables.end() ? it->second : ThemeVariable();
}

void Theme::removeVariable(const String& name) {
    m_variables.erase(name);
    variableChanged.emit(name);
}

std::map<String, ThemeVariable> Theme::variables() const {
    return m_variables;
}

void Theme::setStyle(const String& selector, std::shared_ptr<ThemeStyle> style) {
    m_styles[selector] = style;
    styleChanged.emit(selector);
}

std::shared_ptr<ThemeStyle> Theme::style(const String& selector) const {
    auto it = m_styles.find(selector);
    return it != m_styles.end() ? it->second : nullptr;
}

void Theme::removeStyle(const String& selector) {
    m_styles.erase(selector);
    styleChanged.emit(selector);
}

std::map<String, std::shared_ptr<ThemeStyle>> Theme::styles() const {
    return m_styles;
}

void Theme::setPrimaryColor(const Color& color) {
    setVariable("primary-color", Variant(color.rgba()));
}

Color Theme::primaryColor() const {
    return variable("primary-color").asColor();
}

void Theme::setSecondaryColor(const Color& color) {
    setVariable("secondary-color", Variant(color.rgba()));
}

Color Theme::secondaryColor() const {
    return variable("secondary-color").asColor();
}

void Theme::setBackgroundColor(const Color& color) {
    setVariable("background-color", Variant(color.rgba()));
}

Color Theme::backgroundColor() const {
    return variable("background-color").asColor();
}

void Theme::setSurfaceColor(const Color& color) {
    setVariable("surface-color", Variant(color.rgba()));
}

Color Theme::surfaceColor() const {
    return variable("surface-color").asColor();
}

void Theme::setTextColor(const Color& color) {
    setVariable("text-color", Variant(color.rgba()));
}

Color Theme::textColor() const {
    return variable("text-color").asColor();
}

void Theme::setAccentColor(const Color& color) {
    setVariable("accent-color", Variant(color.rgba()));
}

Color Theme::accentColor() const {
    return variable("accent-color").asColor();
}

void Theme::setIconPath(const String& path) {
    m_iconPath = path;
}

String Theme::iconPath() const {
    return m_iconPath;
}

void Theme::setFontPath(const String& path) {
    m_fontPath = path;
}

String Theme::fontPath() const {
    return m_fontPath;
}

void Theme::setImagePath(const String& path) {
    m_imagePath = path;
}

String Theme::imagePath() const {
    return m_imagePath;
}

bool Theme::loadFromFile(const String& filePath) {
    return false; // 简化实现
}

bool Theme::saveToFile(const String& filePath) const {
    return false; // 简化实现
}

bool Theme::loadFromJSON(const String& json) {
    return false; // 简化实现
}

String Theme::toJSON() const {
    return "{}"; // 简化实现
}

void Theme::applyToWidget(Widget* widget) const {
    if (!widget) return;
    
    widget->setProperty("primary-color", primaryColor());
    widget->setProperty("secondary-color", secondaryColor());
    widget->setProperty("background-color", backgroundColor());
    widget->setProperty("text-color", textColor());
    widget->setProperty("accent-color", accentColor());
    
    String selector = widget->objectName();
    auto style = this->style(selector);
    if (style) {
        style->applyTo(widget);
    }
}

void Theme::applyToApplication() const {
    if (ThemeManager::instance()) {
        ThemeManager::instance()->applyTheme(std::const_pointer_cast<Theme>(shared_from_this()));
    }
}

std::shared_ptr<Theme> Theme::clone() const {
    auto cloned = std::make_shared<Theme>(m_name + "_copy", m_type);
    cloned->m_displayName = m_displayName;
    cloned->m_description = m_description;
    cloned->m_version = m_version;
    cloned->m_author = m_author;
    cloned->m_variables = m_variables;
    cloned->m_styles = m_styles;
    cloned->m_iconPath = m_iconPath;
    cloned->m_fontPath = m_fontPath;
    cloned->m_imagePath = m_imagePath;
    return cloned;
}

// LightTheme实现
LightTheme::LightTheme(Object* parent)
    : Theme("light", ThemeType::Light, parent) {
    setDisplayName("Light Theme");
    setDescription("Default light theme");
    setupDefaultColors();
    setupDefaultStyles();
}

void LightTheme::setupDefaultColors() {
    setPrimaryColor(Color(0, 120, 215));
    setSecondaryColor(Color(108, 117, 125));
    setBackgroundColor(Color(255, 255, 255));
    setSurfaceColor(Color(248, 249, 250));
    setTextColor(Color(33, 37, 41));
    setAccentColor(Color(40, 167, 69));
}

void LightTheme::setupDefaultStyles() {
    auto buttonStyle = std::make_shared<ThemeStyle>("Button");
    buttonStyle->setProperty("background-color", primaryColor());
    buttonStyle->setProperty("color", Color::White);
    buttonStyle->setProperty("border-radius", "4px");
    buttonStyle->setProperty("padding", "8px 16px");
    setStyle("Button", buttonStyle);
    
    auto labelStyle = std::make_shared<ThemeStyle>("Label");
    labelStyle->setProperty("color", textColor());
    labelStyle->setProperty("font-size", "14px");
    setStyle("Label", labelStyle);
    
    auto lineEditStyle = std::make_shared<ThemeStyle>("LineEdit");
    lineEditStyle->setProperty("background-color", Color::White);
    lineEditStyle->setProperty("border", "1px solid #ced4da");
    lineEditStyle->setProperty("border-radius", "4px");
    lineEditStyle->setProperty("padding", "8px");
    setStyle("LineEdit", lineEditStyle);
}

// DarkTheme实现
DarkTheme::DarkTheme(Object* parent)
    : Theme("dark", ThemeType::Dark, parent) {
    setDisplayName("Dark Theme");
    setDescription("Default dark theme");
    setupDefaultColors();
    setupDefaultStyles();
}

void DarkTheme::setupDefaultColors() {
    setPrimaryColor(Color(100, 181, 246));
    setSecondaryColor(Color(158, 158, 158));
    setBackgroundColor(Color(18, 18, 18));
    setSurfaceColor(Color(37, 37, 37));
    setTextColor(Color(255, 255, 255));
    setAccentColor(Color(76, 175, 80));
}

void DarkTheme::setupDefaultStyles() {
    auto buttonStyle = std::make_shared<ThemeStyle>("Button");
    buttonStyle->setProperty("background-color", primaryColor());
    buttonStyle->setProperty("color", Color::Black);
    buttonStyle->setProperty("border-radius", "4px");
    buttonStyle->setProperty("padding", "8px 16px");
    setStyle("Button", buttonStyle);
    
    auto labelStyle = std::make_shared<ThemeStyle>("Label");
    labelStyle->setProperty("color", textColor());
    labelStyle->setProperty("font-size", "14px");
    setStyle("Label", labelStyle);
    
    auto lineEditStyle = std::make_shared<ThemeStyle>("LineEdit");
    lineEditStyle->setProperty("background-color", surfaceColor());
    lineEditStyle->setProperty("border", "1px solid #555");
    lineEditStyle->setProperty("border-radius", "4px");
    lineEditStyle->setProperty("padding", "8px");
    lineEditStyle->setProperty("color", textColor());
    setStyle("LineEdit", lineEditStyle);
}

// ThemeManager私有数据类
class ThemeManager::ThemeManagerPrivate {
public:
    std::map<String, std::shared_ptr<Theme>> themes;
    std::shared_ptr<Theme> currentTheme;
    std::shared_ptr<Theme> previewTheme;
    String currentThemeName;
    
    ThemeMode themeMode = ThemeMode::Static;
    bool followSystemTheme = false;
    
    String lightThemeName = "light";
    String darkThemeName = "dark";
    int lightThemeHour = 6;
    int lightThemeMinute = 0;
    int darkThemeHour = 18;
    int darkThemeMinute = 0;
    
    bool previewMode = false;
    bool transitionEnabled = true;
    int transitionDuration = 300;
    
    String themeDirectory = "./themes";
    bool cacheEnabled = true;
    std::map<String, std::shared_ptr<ThemeStyle>> styleCache;
    
    std::map<String, Variant> globalVariables;
    
    Timer* scheduleTimer = nullptr;
    Timer* systemThemeTimer = nullptr;
    
    ThemeManagerPrivate() = default;
};

ThemeManager::ThemeManager(Object* parent)
    : Object(parent)
    , d_ptr(std::make_unique<ThemeManagerPrivate>()) {
    initializeThemeManager();
}

ThemeManager::~ThemeManager() = default;

void ThemeManager::initializeThemeManager() {
    setObjectName("ThemeManager");
    setupDefaultThemes();
    setCurrentTheme("light");
}

ThemeManager* ThemeManager::instance() {
    return s_instance;
}

void ThemeManager::setInstance(ThemeManager* manager) {
    s_instance = manager;
}

void ThemeManager::registerTheme(std::shared_ptr<Theme> theme) {
    if (theme) {
        d_ptr->themes[theme->name()] = theme;
    }
}

void ThemeManager::unregisterTheme(const String& name) {
    d_ptr->themes.erase(name);
    
    if (d_ptr->currentThemeName == name) {
        setCurrentTheme("light");
    }
}

bool ThemeManager::isThemeRegistered(const String& name) const {
    return d_ptr->themes.find(name) != d_ptr->themes.end();
}

std::shared_ptr<Theme> ThemeManager::theme(const String& name) const {
    auto it = d_ptr->themes.find(name);
    return it != d_ptr->themes.end() ? it->second : nullptr;
}

StringList ThemeManager::availableThemes() const {
    StringList names;
    for (const auto& pair : d_ptr->themes) {
        names.push_back(pair.first);
    }
    return names;
}

void ThemeManager::setCurrentTheme(const String& name) {
    auto theme = this->theme(name);
    if (theme) {
        setCurrentTheme(theme);
    }
}

void ThemeManager::setCurrentTheme(std::shared_ptr<Theme> theme) {
    if (!theme || d_ptr->currentTheme == theme) {
        return;
    }
    
    d_ptr->currentTheme = theme;
    d_ptr->currentThemeName = theme->name();
    
    applyTheme(theme, d_ptr->transitionEnabled);
    themeChanged.emit(theme->name());
}

std::shared_ptr<Theme> ThemeManager::currentTheme() const {
    return d_ptr->previewMode ? d_ptr->previewTheme : d_ptr->currentTheme;
}

String ThemeManager::currentThemeName() const {
    auto theme = currentTheme();
    return theme ? theme->name() : "";
}

void ThemeManager::setThemeMode(ThemeMode mode) {
    d_ptr->themeMode = mode;
    themeModeChanged.emit(mode);
}

ThemeMode ThemeManager::themeMode() const {
    return d_ptr->themeMode;
}

void ThemeManager::setScheduledThemes(const String& lightTheme, const String& darkTheme) {
    d_ptr->lightThemeName = lightTheme;
    d_ptr->darkThemeName = darkTheme;
}

void ThemeManager::setLightThemeTime(int hour, int minute) {
    d_ptr->lightThemeHour = hour;
    d_ptr->lightThemeMinute = minute;
}

void ThemeManager::setDarkThemeTime(int hour, int minute) {
    d_ptr->darkThemeHour = hour;
    d_ptr->darkThemeMinute = minute;
}

void ThemeManager::setFollowSystemTheme(bool follow) {
    d_ptr->followSystemTheme = follow;
}

bool ThemeManager::followSystemTheme() const {
    return d_ptr->followSystemTheme;
}

void ThemeManager::previewTheme(const String& name) {
    auto theme = this->theme(name);
    if (theme) {
        d_ptr->previewTheme = theme;
        d_ptr->previewMode = true;
        
        applyTheme(theme, d_ptr->transitionEnabled);
        previewStarted.emit(name);
    }
}

void ThemeManager::endPreview() {
    if (d_ptr->previewMode) {
        d_ptr->previewMode = false;
        d_ptr->previewTheme = nullptr;
        
        if (d_ptr->currentTheme) {
            applyTheme(d_ptr->currentTheme, d_ptr->transitionEnabled);
        }
        
        previewEnded.emit();
    }
}

bool ThemeManager::isPreviewMode() const {
    return d_ptr->previewMode;
}

void ThemeManager::setThemeDirectory(const String& directory) {
    d_ptr->themeDirectory = directory;
}

String ThemeManager::themeDirectory() const {
    return d_ptr->themeDirectory;
}

void ThemeManager::loadThemesFromDirectory() {
    // 简化实现
}

void ThemeManager::loadThemeFromFile(const String& filePath) {
    // 简化实现
}

std::shared_ptr<Theme> ThemeManager::createCustomTheme(const String& name, std::shared_ptr<Theme> baseTheme) {
    auto customTheme = baseTheme ? baseTheme->clone() : std::make_shared<Theme>();
    customTheme->setName(name);
    customTheme->setType(ThemeType::Custom);
    
    registerTheme(customTheme);
    return customTheme;
}

void ThemeManager::saveCustomTheme(std::shared_ptr<Theme> theme) {
    // 简化实现
}

void ThemeManager::deleteCustomTheme(const String& name) {
    auto theme = this->theme(name);
    if (theme && theme->type() == ThemeType::Custom) {
        unregisterTheme(name);
    }
}

void ThemeManager::setGlobalVariable(const String& name, const Variant& value) {
    d_ptr->globalVariables[name] = value;
    globalVariableChanged.emit(name);
}

Variant ThemeManager::globalVariable(const String& name) const {
    auto it = d_ptr->globalVariables.find(name);
    return it != d_ptr->globalVariables.end() ? it->second : Variant();
}

void ThemeManager::removeGlobalVariable(const String& name) {
    d_ptr->globalVariables.erase(name);
    globalVariableChanged.emit(name);
}

std::shared_ptr<ThemeStyle> ThemeManager::selectStyle(Widget* widget) const {
    if (!widget) return nullptr;
    
    String selector = generateStyleSelector(widget);
    return selectStyle(selector);
}

std::shared_ptr<ThemeStyle> ThemeManager::selectStyle(const String& selector) const {
    if (d_ptr->cacheEnabled) {
        auto it = d_ptr->styleCache.find(selector);
        if (it != d_ptr->styleCache.end()) {
            return it->second;
        }
    }
    
    auto theme = currentTheme();
    if (!theme) return nullptr;
    
    auto style = theme->style(selector);
    
    if (d_ptr->cacheEnabled && style) {
        d_ptr->styleCache[selector] = style;
    }
    
    return style;
}

void ThemeManager::setThemeTransitionEnabled(bool enabled) {
    d_ptr->transitionEnabled = enabled;
}

bool ThemeManager::isThemeTransitionEnabled() const {
    return d_ptr->transitionEnabled;
}

void ThemeManager::setTransitionDuration(int duration) {
    d_ptr->transitionDuration = std::max(0, duration);
}

int ThemeManager::transitionDuration() const {
    return d_ptr->transitionDuration;
}

void ThemeManager::clearStyleCache() {
    d_ptr->styleCache.clear();
}

void ThemeManager::setCacheEnabled(bool enabled) {
    d_ptr->cacheEnabled = enabled;
    
    if (!enabled) {
        clearStyleCache();
    }
}

bool ThemeManager::isCacheEnabled() const {
    return d_ptr->cacheEnabled;
}

// 内部方法实现
void ThemeManager::applyTheme(std::shared_ptr<Theme> theme, bool animated) {
    if (!theme) return;
    
    clearStyleCache();
    
    // 这里应该遍历所有控件并应用主题
    // 简化实现
}

void ThemeManager::updateScheduledTheme() {
    // 简化实现
}

void ThemeManager::detectSystemTheme() {
    // 简化实现
    if (d_ptr->followSystemTheme) {
        setCurrentTheme("light");
    }
}

std::shared_ptr<ThemeStyle> ThemeManager::computeEffectiveStyle(Widget* widget) const {
    return selectStyle(widget);
}

String ThemeManager::generateStyleSelector(Widget* widget) const {
    if (!widget) return "";
    return widget->objectName();
}

void ThemeManager::setupDefaultThemes() {
    registerTheme(std::make_shared<LightTheme>());
    registerTheme(std::make_shared<DarkTheme>());
}

void ThemeManager::setupSystemThemeDetection() {
    // 简化实现
}

void ThemeManager::setupScheduledThemeSwitch() {
    // 简化实现
}

// 便利函数实现
namespace Theme {
    void applyToWidget(Widget* widget) {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->currentTheme();
            if (theme) {
                theme->applyToWidget(widget);
            }
        }
    }
    
    void applyToWidget(Widget* widget, const String& themeName) {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->theme(themeName);
            if (theme) {
                theme->applyToWidget(widget);
            }
        }
    }
    
    Color primaryColor() {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->currentTheme();
            if (theme) {
                return theme->primaryColor();
            }
        }
        return Color(0, 120, 215);
    }
    
    Color secondaryColor() {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->currentTheme();
            if (theme) {
                return theme->secondaryColor();
            }
        }
        return Color(108, 117, 125);
    }
    
    Color backgroundColor() {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->currentTheme();
            if (theme) {
                return theme->backgroundColor();
            }
        }
        return Color::White;
    }
    
    Color textColor() {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->currentTheme();
            if (theme) {
                return theme->textColor();
            }
        }
        return Color::Black;
    }
    
    Color accentColor() {
        if (auto manager = ThemeManager::instance()) {
            auto theme = manager->currentTheme();
            if (theme) {
                return theme->accentColor();
            }
        }
        return Color(40, 167, 69);
    }
    
    void setLightTheme() {
        if (auto manager = ThemeManager::instance()) {
            manager->setCurrentTheme("light");
        }
    }
    
    void setDarkTheme() {
        if (auto manager = ThemeManager::instance()) {
            manager->setCurrentTheme("dark");
        }
    }
    
    void toggleTheme() {
        if (auto manager = ThemeManager::instance()) {
            String current = manager->currentThemeName();
            if (current == "light") {
                setDarkTheme();
            } else {
                setLightTheme();
            }
        }
    }
}

} // namespace QGL