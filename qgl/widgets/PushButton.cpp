#include "PushButton.h"
#include "../timer/Timer.h"
#include <algorithm>

namespace QGL {

// 静态成员初始化
std::map<String, std::vector<PushButton*>> PushButton::s_buttonGroups;

PushButton::PushButton(Widget* parent) : Widget(parent) {
    initializeStyles();
    
    // 设置默认属性
    setProperty("text", Variant::fromString(""));
    setProperty("checkable", Variant::fromBool(false));
    setProperty("checked", Variant::fromBool(false));
    setProperty("flat", Variant::fromBool(false));
    
    // 设置焦点策略
    setFocusPolicy(FocusPolicy::StrongFocus);
    
    updateButtonStyle();
}

PushButton::PushButton(const String& text, Widget* parent) : Widget(parent), m_text(text) {
    initializeStyles();
    
    // 设置默认属性
    setProperty("text", Variant::fromString(text));
    setProperty("checkable", Variant::fromBool(false));
    setProperty("checked", Variant::fromBool(false));
    setProperty("flat", Variant::fromBool(false));
    
    // 设置焦点策略
    setFocusPolicy(FocusPolicy::StrongFocus);
    
    updateButtonStyle();
}

PushButton::~PushButton() {
    removeFromButtonGroup();
    destroyAutoRepeatTimer();
    cleanupStyles();
}

lv_obj_t* PushButton::createLvglObject(lv_obj_t* parent) {
    return lv_btn_create(parent);
}

void PushButton::setText(const String& text) {
    if (m_text != text) {
        m_text = text;
        setProperty("text", Variant::fromString(text));
        
        if (lvglObject()) {
            lv_obj_t* label = lv_obj_get_child(lvglObject(), 0);
            if (label) {
                lv_label_set_text(label, text.c_str());
            } else {
                // 创建标签
                label = lv_label_create(lvglObject());
                lv_label_set_text(label, text.c_str());
                lv_obj_center(label);
            }
        }
    }
}

String PushButton::getText() const {
    return m_text;
}

void PushButton::setIcon(const String& iconPath) {
    m_iconPath = iconPath;
    setProperty("icon", Variant::fromString(iconPath));
    // TODO: 实现图标显示
}

String PushButton::getIcon() const {
    return m_iconPath;
}

void PushButton::setIconSize(const Size& size) {
    m_iconSize = size;
    setProperty("iconSize", Variant::fromSize(size));
    // TODO: 更新图标尺寸
}

Size PushButton::getIconSize() const {
    return m_iconSize;
}

void PushButton::setButtonStyle(ButtonStyle style) {
    if (m_buttonStyle != style) {
        m_buttonStyle = style;
        setProperty("buttonStyle", Variant::fromInt(static_cast<int>(style)));
        updateButtonStyle();
    }
}

ButtonStyle PushButton::getButtonStyle() const {
    return m_buttonStyle;
}

ButtonState PushButton::getButtonState() const {
    return m_buttonState;
}

void PushButton::setCheckable(bool checkable) {
    if (m_checkable != checkable) {
        m_checkable = checkable;
        setProperty("checkable", Variant::fromBool(checkable));
        
        if (checkable) {
            addToButtonGroup();
        } else {
            removeFromButtonGroup();
            setChecked(false);
        }
    }
}

bool PushButton::isCheckable() const {
    return m_checkable;
}

void PushButton::setChecked(bool checked) {
    if (m_checkable && m_checked != checked) {
        m_checked = checked;
        setProperty("checked", Variant::fromBool(checked));
        
        if (checked && !m_buttonGroup.empty()) {
            uncheckOthersInGroup();
        }
        
        updateButtonState(checked ? ButtonState::Pressed : ButtonState::Normal);
        toggled.emit(checked);
    }
}

bool PushButton::isChecked() const {
    return m_checked;
}

void PushButton::setFlat(bool flat) {
    if (m_flat != flat) {
        m_flat = flat;
        setProperty("flat", Variant::fromBool(flat));
        updateButtonStyle();
    }
}

bool PushButton::isFlat() const {
    return m_flat;
}

void PushButton::setDefault(bool isDefault) {
    if (m_isDefault != isDefault) {
        m_isDefault = isDefault;
        setProperty("default", Variant::fromBool(isDefault));
        updateButtonStyle();
    }
}

bool PushButton::isDefault() const {
    return m_isDefault;
}

void PushButton::setAutoRepeat(bool autoRepeat) {
    if (m_autoRepeat != autoRepeat) {
        m_autoRepeat = autoRepeat;
        setProperty("autoRepeat", Variant::fromBool(autoRepeat));
        
        if (autoRepeat) {
            createAutoRepeatTimer();
        } else {
            destroyAutoRepeatTimer();
        }
    }
}

bool PushButton::getAutoRepeat() const {
    return m_autoRepeat;
}

void PushButton::setAutoRepeatDelay(int delay) {
    m_autoRepeatDelay = delay;
    setProperty("autoRepeatDelay", Variant::fromInt(delay));
}

int PushButton::getAutoRepeatDelay() const {
    return m_autoRepeatDelay;
}

void PushButton::setAutoRepeatInterval(int interval) {
    m_autoRepeatInterval = interval;
    setProperty("autoRepeatInterval", Variant::fromInt(interval));
}

int PushButton::getAutoRepeatInterval() const {
    return m_autoRepeatInterval;
}

void PushButton::setButtonGroup(const String& groupName) {
    if (m_buttonGroup != groupName) {
        removeFromButtonGroup();
        m_buttonGroup = groupName;
        setProperty("buttonGroup", Variant::fromString(groupName));
        if (m_checkable) {
            addToButtonGroup();
        }
    }
}

String PushButton::getButtonGroup() const {
    return m_buttonGroup;
}

void PushButton::setShortcut(const String& shortcut) {
    m_shortcut = shortcut;
    setProperty("shortcut", Variant::fromString(shortcut));
    // TODO: 实现快捷键功能
}

String PushButton::getShortcut() const {
    return m_shortcut;
}

void PushButton::setTextColor(const Color& color) {
    m_textColor = color;
    setProperty("textColor", Variant::fromColor(color));
    updateButtonStyle();
}

Color PushButton::getTextColor() const {
    return m_textColor;
}

void PushButton::setBackgroundColorNormal(const Color& color) {
    m_backgroundColorNormal = color;
    setProperty("backgroundColorNormal", Variant::fromColor(color));
    updateButtonStyle();
}

Color PushButton::getBackgroundColorNormal() const {
    return m_backgroundColorNormal;
}

void PushButton::setBackgroundColorHovered(const Color& color) {
    m_backgroundColorHovered = color;
    setProperty("backgroundColorHovered", Variant::fromColor(color));
    updateButtonStyle();
}

Color PushButton::getBackgroundColorHovered() const {
    return m_backgroundColorHovered;
}

void PushButton::setBackgroundColorPressed(const Color& color) {
    m_backgroundColorPressed = color;
    setProperty("backgroundColorPressed", Variant::fromColor(color));
    updateButtonStyle();
}

Color PushButton::getBackgroundColorPressed() const {
    return m_backgroundColorPressed;
}

void PushButton::click() {
    if (!isEnabled()) return;
    
    // 程序化点击
    if (m_checkable) {
        setChecked(!m_checked);
    }
    
    if (m_animationEnabled) {
        startClickAnimation();
    }
    
    clicked.emit();
}

void PushButton::mouseClickEvent(const Point& pos) {
    if (!isEnabled()) return;
    
    Widget::mouseClickEvent(pos);
    
    if (m_checkable) {
        setChecked(!m_checked);
    }
    
    if (m_animationEnabled) {
        startClickAnimation();
    }
    
    clicked.emit();
}

void PushButton::mousePressEvent(const Point& pos) {
    if (!isEnabled()) return;
    
    Widget::mousePressEvent(pos);
    
    updateButtonState(ButtonState::Pressed);
    pressed.emit();
    
    // 启动自动重复
    if (m_autoRepeat && m_autoRepeatTimer) {
        m_autoRepeatTimer->setInterval(m_autoRepeatDelay);
        m_autoRepeatTimer->start();
    }
}

void PushButton::mouseReleaseEvent(const Point& pos) {
    if (!isEnabled()) return;
    
    Widget::mouseReleaseEvent(pos);
    
    updateButtonState(ButtonState::Normal);
    released.emit();
    
    // 停止自动重复
    if (m_autoRepeat && m_autoRepeatTimer) {
        m_autoRepeatTimer->stop();
    }
}

void PushButton::focusInEvent() {
    Widget::focusInEvent();
    updateButtonState(ButtonState::Focused);
}

void PushButton::focusOutEvent() {
    Widget::focusOutEvent();
    updateButtonState(ButtonState::Normal);
}

void PushButton::onPropertyChanged(const String& name, const Variant& value) {
    Widget::onPropertyChanged(name, value);
    
    if (name == "text") {
        m_text = value.toString();
        setText(m_text);
    } else if (name == "checkable") {
        setCheckable(value.toBool());
    } else if (name == "checked") {
        setChecked(value.toBool());
    } else if (name == "flat") {
        setFlat(value.toBool());
    }
}

void PushButton::initializeStyles() {
    if (!m_stylesInitialized) {
        lv_style_init(&m_normalStyle);
        lv_style_init(&m_pressedStyle);
        lv_style_init(&m_disabledStyle);
        m_stylesInitialized = true;
    }
}

void PushButton::cleanupStyles() {
    if (m_stylesInitialized) {
        lv_style_reset(&m_normalStyle);
        lv_style_reset(&m_pressedStyle);
        lv_style_reset(&m_disabledStyle);
        m_stylesInitialized = false;
    }
}

void PushButton::updateButtonState(ButtonState newState) {
    if (m_buttonState != newState) {
        ButtonState oldState = m_buttonState;
        m_buttonState = newState;
        
        applyStateStyle();
        stateChanged.emit(newState);
        
        if (m_animationEnabled) {
            animateToState(newState);
        }
    }
}

void PushButton::updateButtonStyle() {
    if (!lvglObject() || !m_stylesInitialized) return;
    
    // 设置正常状态样式
    lv_style_set_bg_color(&m_normalStyle, lv_color_hex(m_backgroundColorNormal.rgba() >> 8));
    lv_style_set_text_color(&m_normalStyle, lv_color_hex(m_textColor.rgba() >> 8));
    lv_style_set_border_color(&m_normalStyle, lv_color_hex(m_borderColorNormal.rgba() >> 8));
    
    if (m_flat) {
        lv_style_set_bg_opa(&m_normalStyle, LV_OPA_TRANSP);
        lv_style_set_border_opa(&m_normalStyle, LV_OPA_TRANSP);
    } else {
        lv_style_set_bg_opa(&m_normalStyle, LV_OPA_COVER);
        lv_style_set_border_opa(&m_normalStyle, LV_OPA_COVER);
        lv_style_set_border_width(&m_normalStyle, getBorderWidth());
    }
    
    // 设置按下状态样式
    lv_style_set_bg_color(&m_pressedStyle, lv_color_hex(m_backgroundColorPressed.rgba() >> 8));
    lv_style_set_text_color(&m_pressedStyle, lv_color_hex(m_textColorPressed.rgba() >> 8));
    lv_style_set_border_color(&m_pressedStyle, lv_color_hex(m_borderColorPressed.rgba() >> 8));
    
    // 设置禁用状态样式
    lv_style_set_bg_color(&m_disabledStyle, lv_color_hex(m_backgroundColorDisabled.rgba() >> 8));
    lv_style_set_text_color(&m_disabledStyle, lv_color_hex(m_textColorDisabled.rgba() >> 8));
    lv_style_set_bg_opa(&m_disabledStyle, LV_OPA_50);
    
    // 应用当前状态样式
    applyStateStyle();
}

void PushButton::applyStateStyle() {
    if (!lvglObject()) return;
    
    // 移除所有状态样式
    lv_obj_remove_style_all(lvglObject());
    
    // 应用基础样式
    lv_obj_add_style(lvglObject(), &m_normalStyle, 0);
    
    // 根据状态应用额外样式
    switch (m_buttonState) {
    case ButtonState::Pressed:
        lv_obj_add_style(lvglObject(), &m_pressedStyle, LV_STATE_PRESSED);
        break;
    case ButtonState::Disabled:
        lv_obj_add_style(lvglObject(), &m_disabledStyle, LV_STATE_DISABLED);
        break;
    case ButtonState::Hovered:
        // 悬停样式可以在这里添加
        break;
    case ButtonState::Focused:
        lv_obj_add_state(lvglObject(), LV_STATE_FOCUSED);
        break;
    default:
        break;
    }
}

void PushButton::createAutoRepeatTimer() {
    if (!m_autoRepeatTimer) {
        m_autoRepeatTimer = new Timer(m_autoRepeatInterval, TimerType::Repeating, this);
        m_autoRepeatTimer->setCallback([this]() {
            handleAutoRepeat();
        });
    }
}

void PushButton::destroyAutoRepeatTimer() {
    if (m_autoRepeatTimer) {
        delete m_autoRepeatTimer;
        m_autoRepeatTimer = nullptr;
    }
}

void PushButton::handleAutoRepeat() {
    if (m_buttonState == ButtonState::Pressed) {
        // 切换到重复间隔
        if (m_autoRepeatTimer->getInterval() != m_autoRepeatInterval) {
            m_autoRepeatTimer->setInterval(m_autoRepeatInterval);
        }
        
        clicked.emit();
    }
}

void PushButton::addToButtonGroup() {
    if (!m_buttonGroup.empty() && m_checkable) {
        auto& group = s_buttonGroups[m_buttonGroup];
        if (std::find(group.begin(), group.end(), this) == group.end()) {
            group.push_back(this);
        }
    }
}

void PushButton::removeFromButtonGroup() {
    if (!m_buttonGroup.empty()) {
        auto& group = s_buttonGroups[m_buttonGroup];
        group.erase(std::remove(group.begin(), group.end(), this), group.end());
        
        if (group.empty()) {
            s_buttonGroups.erase(m_buttonGroup);
        }
    }
}

void PushButton::uncheckOthersInGroup() {
    if (!m_buttonGroup.empty()) {
        auto& group = s_buttonGroups[m_buttonGroup];
        for (PushButton* button : group) {
            if (button != this && button->isChecked()) {
                button->setChecked(false);
            }
        }
    }
}

void PushButton::startClickAnimation() {
    // 简单的点击动画：缩放效果
    if (lvglObject()) {
        // TODO: 实现点击动画效果
        // 可以使用 LVGL 的动画 API 或者自定义动画定时器
    }
}

void PushButton::animateToState(ButtonState targetState) {
    // TODO: 实现状态切换动画
    // 可以使用颜色渐变、透明度变化等效果
}

} // namespace QGL