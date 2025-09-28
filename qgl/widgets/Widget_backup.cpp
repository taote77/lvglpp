#include "Widget.h"
#include "../layouts/Layout.h"
#include <algorithm>

namespace QGL {

Widget::Widget(Widget* parent) : Object(parent) {
    // 初始化 LVGL 对象
    initializeLvglObject();
    
    // 设置默认属性
    setProperty("x", Variant::fromInt(0));
    setProperty("y", Variant::fromInt(0));
    setProperty("width", Variant::fromInt(100));
    setProperty("height", Variant::fromInt(30));
    setProperty("visible", Variant::fromBool(true));
    setProperty("enabled", Variant::fromBool(true));
    setProperty("backgroundColor", Variant::fromColor(Color::White));
    setProperty("opacity", Variant::fromFloat(1.0f));
}

Widget::~Widget() {
    // 清理布局
    if (m_layout) {
        delete m_layout;
        m_layout = nullptr;
    }
    
    // 删除 LVGL 对象
    if (m_lvglObj) {
        lv_obj_del(m_lvglObj);
        m_lvglObj = nullptr;
    }
}

void Widget::initializeLvglObject() {
    lv_obj_t* parentObj = nullptr;
    if (parentWidget()) {
        parentObj = parentWidget()->lvglObject();
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

void Widget::setGeometry(const Rect& rect) {
    Rect oldGeometry = getGeometry();
    
    setProperty("x", Variant::fromInt(rect.x));
    setProperty("y", Variant::fromInt(rect.y));
    setProperty("width", Variant::fromInt(rect.width));
    setProperty("height", Variant::fromInt(rect.height));
    
    if (m_lvglObj) {
        lv_obj_set_pos(m_lvglObj, rect.x, rect.y);
        lv_obj_set_size(m_lvglObj, rect.width, rect.height);
    }
    
    if (oldGeometry.x != rect.x || oldGeometry.y != rect.y) {
        moveEvent(rect.topLeft(), oldGeometry.topLeft());
    }
    
    if (oldGeometry.width != rect.width || oldGeometry.height != rect.height) {
        resizeEvent(rect.size(), oldGeometry.size());
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
    Rect geometry = getGeometry();
    geometry.x = x;
    setGeometry(geometry);
}

int Widget::getX() const {
    return getProperty("x").toInt();
}

void Widget::setY(int y) {
    Rect geometry = getGeometry();
    geometry.y = y;
    setGeometry(geometry);
}

int Widget::getY() const {
    return getProperty("y").toInt();
}

void Widget::setWidth(int width) {
    Rect geometry = getGeometry();
    geometry.width = width;
    setGeometry(geometry);
}

int Widget::getWidth() const {
    return getProperty("width").toInt();
}

void Widget::setHeight(int height) {
    Rect geometry = getGeometry();
    geometry.height = height;
    setGeometry(geometry);
}

int Widget::getHeight() const {
    return getProperty("height").toInt();
}

void Widget::setVisible(bool visible) {
    bool oldVisible = isVisible();
    setProperty("visible", Variant::fromBool(visible));
    
    if (m_lvglObj) {
        if (visible) {
            lv_obj_clear_flag(m_lvglObj, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(m_lvglObj, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    if (oldVisible != visible) {
        visibilityChanged.emit(visible);
        if (visible) {
            showEvent();
        } else {
            hideEvent();
        }
    }
}

bool Widget::isVisible() const {
    return getProperty("visible").toBool();
}

void Widget::setEnabled(bool enabled) {
    bool oldEnabled = isEnabled();
    setProperty("enabled", Variant::fromBool(enabled));
    
    if (m_lvglObj) {
        if (enabled) {
            lv_obj_clear_state(m_lvglObj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_lvglObj, LV_STATE_DISABLED);
        }
    }
    
    if (oldEnabled != enabled) {
        enabledChanged.emit(enabled);
    }
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

void Widget::setLayout(Layout* layout) {
    if (m_layout) {
        delete m_layout;
    }
    
    m_layout = layout;
    if (m_layout) {
        m_layout->setParentWidget(this);
        m_layout->activate();
    }
}

Layout* Widget::getLayout() const {
    return m_layout;
}

Widget* Widget::parentWidget() const {
    return dynamic_cast<Widget*>(parent());
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
    paintEvent();
    update();
}

void Widget::setStyleSheet(const String& css) {
    m_styleSheet = css;
    // TODO: 解析 CSS 并应用样式
    applyStylesToLvgl();
}

String Widget::getStyleSheet() const {
    return m_styleSheet;
}

void Widget::applyStylesToLvgl() {
    if (!m_lvglObj) return;
    
    // 应用基本样式
    Color bgColor = getBackgroundColor();
    lv_obj_set_style_bg_color(m_lvglObj, lv_color_hex(bgColor.rgba() >> 8), 0);
    lv_obj_set_style_bg_opa(m_lvglObj, bgColor.alpha(), 0);
    
    float opacity = getOpacity();
    lv_obj_set_style_opa(m_lvglObj, static_cast<lv_opa_t>(opacity * 255), 0);
}

void Widget::lvglEventCallback(lv_event_t* e) {
    Widget* widget = static_cast<Widget*>(lv_event_get_user_data(e));
    if (widget) {
        widget->handleLvglEvent(lv_event_get_code(e));
    }
}

void Widget::handleLvglEvent(lv_event_code_t eventCode) {
    switch (eventCode) {
    case LV_EVENT_CLICKED:
        mouseClickEvent(Point(0, 0)); // TODO: 获取实际点击位置
        break;
    case LV_EVENT_PRESSED:
        mousePressEvent(Point(0, 0));
        break;
    case LV_EVENT_RELEASED:
        mouseReleaseEvent(Point(0, 0));
        break;
    case LV_EVENT_FOCUSED:
        focusChanged.emit(true);
        focusInEvent();
        break;
    case LV_EVENT_DEFOCUSED:
        focusChanged.emit(false);
        focusOutEvent();
        break;
    default:
        break;
    }
}

} // namespace QGL