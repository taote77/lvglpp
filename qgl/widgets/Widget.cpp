#include "Widget.h"
#include "../layouts/Layout.h"

namespace QGL {

Widget::Widget(Widget* parent) : Object(parent) {
    setObjectName("Widget");
    
    // 初始化默认属性
    setProperty("x", Variant::fromInt(0));
    setProperty("y", Variant::fromInt(0));
    setProperty("width", Variant::fromInt(100));
    setProperty("height", Variant::fromInt(100));
    setProperty("visible", Variant::fromBool(true));
    setProperty("enabled", Variant::fromBool(true));
    setProperty("backgroundColor", Variant::fromColor(Color::White));
    setProperty("opacity", Variant::fromFloat(1.0f));
    
    // 初始化 LVGL 对象
    initializeLvglObject();
    
    // 如果有父控件，添加到父控件的子控件列表
    if (parent) {
        parent->m_childWidgets.push_back(this);
    }
}

Widget::~Widget() {
    // 清理布局
    if (m_layout) {
        delete m_layout;
    }
    
    // 清理 LVGL 对象
    if (m_lvglObj) {
        lv_obj_del(m_lvglObj);
        m_lvglObj = nullptr;
    }
}

void Widget::initializeLvglObject() {
    lv_obj_t* parentObj = nullptr;
    Widget* parent = parentWidget();
    if (parent) {
        parentObj = parent->lvglObject();
    }
    
    m_lvglObj = createLvglObject(parentObj);
    
    if (m_lvglObj) {
        // 设置用户数据
        lv_obj_set_user_data(m_lvglObj, this);
        
        // 添加事件回调
        lv_obj_add_event_cb(m_lvglObj, lvglEventCallback, LV_EVENT_ALL, this);
        
        // 应用初始样式
        applyStylesToLvgl();
    }
}

lv_obj_t* Widget::createLvglObject(lv_obj_t* parent) {
    return lv_obj_create(parent);
}

// 几何属性方法的简化实现
void Widget::setGeometry(const Rect& rect) {
    setProperty("x", Variant::fromInt(rect.x));
    setProperty("y", Variant::fromInt(rect.y));
    setProperty("width", Variant::fromInt(rect.width));
    setProperty("height", Variant::fromInt(rect.height));
    
    if (m_lvglObj) {
        lv_obj_set_pos(m_lvglObj, rect.x, rect.y);
        lv_obj_set_size(m_lvglObj, rect.width, rect.height);
    }
    
    geometryChanged.emit(rect);
}

Rect Widget::getGeometry() const {
    return Rect(
        getProperty("x").toInt(),
        getProperty("y").toInt(),
        getProperty("width").toInt(),
        getProperty("height").toInt()
    );
}

void Widget::setPosition(const Point& pos) {
    setX(pos.x);
    setY(pos.y);
}

Point Widget::getPosition() const {
    return Point(getX(), getY());
}

void Widget::setSize(const Size& size) {
    setWidth(size.width);
    setHeight(size.height);
}

Size Widget::getSize() const {
    return Size(getWidth(), getHeight());
}

void Widget::setX(int x) {
    setProperty("x", Variant::fromInt(x));
    if (m_lvglObj) {
        lv_obj_set_x(m_lvglObj, x);
    }
}

int Widget::getX() const {
    return getProperty("x").toInt();
}

void Widget::setY(int y) {
    setProperty("y", Variant::fromInt(y));
    if (m_lvglObj) {
        lv_obj_set_y(m_lvglObj, y);
    }
}

int Widget::getY() const {
    return getProperty("y").toInt();
}

void Widget::setWidth(int width) {
    setProperty("width", Variant::fromInt(width));
    if (m_lvglObj) {
        lv_obj_set_width(m_lvglObj, width);
    }
}

int Widget::getWidth() const {
    return getProperty("width").toInt();
}

void Widget::setHeight(int height) {
    setProperty("height", Variant::fromInt(height));
    if (m_lvglObj) {
        lv_obj_set_height(m_lvglObj, height);
    }
}

int Widget::getHeight() const {
    return getProperty("height").toInt();
}

void Widget::setVisible(bool visible) {
    setProperty("visible", Variant::fromBool(visible));
    
    if (m_lvglObj) {
        if (visible) {
            lv_obj_clear_flag(m_lvglObj, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(m_lvglObj, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    visibilityChanged.emit(visible);
}

bool Widget::isVisible() const {
    return getProperty("visible").toBool();
}

void Widget::setEnabled(bool enabled) {
    setProperty("enabled", Variant::fromBool(enabled));
    
    if (m_lvglObj) {
        if (enabled) {
            lv_obj_clear_state(m_lvglObj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_lvglObj, LV_STATE_DISABLED);
        }
    }
    
    enabledChanged.emit(enabled);
}

bool Widget::isEnabled() const {
    return getProperty("enabled").toBool();
}

void Widget::setBackgroundColor(const Color& color) {
    setProperty("backgroundColor", Variant::fromColor(color));
    
    if (m_lvglObj) {
        lv_obj_set_style_bg_color(m_lvglObj, lv_color_hex(color.rgba() >> 8), 0);
        lv_obj_set_style_bg_opa(m_lvglObj, color.alpha(), 0);
    }
}

Color Widget::getBackgroundColor() const {
    return getProperty("backgroundColor").getValue<Color>();
}

void Widget::setOpacity(float opacity) {
    setProperty("opacity", Variant::fromFloat(opacity));
    
    if (m_lvglObj) {
        lv_obj_set_style_opa(m_lvglObj, static_cast<lv_opa_t>(opacity * 255), 0);
    }
}

float Widget::getOpacity() const {
    return getProperty("opacity").toFloat();
}

// 其他简化方法的实现
void Widget::setLayout(Layout* layout) {
    if (m_layout) {
        delete m_layout;
    }
    m_layout = layout;
}

Layout* Widget::getLayout() const {
    return m_layout;
}

Widget* Widget::parentWidget() const {
    return static_cast<Widget*>(parent());
}

const std::vector<Widget*>& Widget::childWidgets() const {
    return m_childWidgets;
}

void Widget::update() {
    if (m_lvglObj) {
        lv_obj_invalidate(m_lvglObj);
    }
}

void Widget::repaint() {
    update();
}

void Widget::setStyleSheet(const String& css) {
    m_styleSheet = css;
}

String Widget::getStyleSheet() const {
    return m_styleSheet;
}

// LVGL 事件处理的简化实现
void Widget::lvglEventCallback(lv_event_t* e) {
    Widget* widget = static_cast<Widget*>(lv_event_get_user_data(e));
    if (widget) {
        lv_event_code_t code = lv_event_get_code(e);
        widget->handleLvglEvent(code);
    }
}

void Widget::applyStylesToLvgl() {
    if (!m_lvglObj) {
        return;
    }
    
    // 应用基本样式
    setBackgroundColor(getBackgroundColor());
    setOpacity(getOpacity());
}

void Widget::updateChildWidgets() {
    // 简化实现，空方法
}

void Widget::handleLvglEvent(lv_event_code_t eventCode) {
    // 简化的事件处理
    switch (eventCode) {
    case LV_EVENT_PRESSED:
    case LV_EVENT_RELEASED:
    case LV_EVENT_CLICKED:
    default:
        break;
    }
}

} // namespace QGL