#ifndef QGL_WIDGETS_WIDGET_H
#define QGL_WIDGETS_WIDGET_H

#include "../core/Object.h"
#include "../core/Types.h"
#include <lvgl.h>

namespace QGL {

// 前向声明
class Layout;

// 控件基类
class Widget : public Object {
    QGL_OBJECT(Widget)
    
public:
    explicit Widget(Widget* parent = nullptr);
    ~Widget() override;
    
    // 几何属性
    void setGeometry(const Rect& rect);
    Rect getGeometry() const;
    
    void setPosition(const Point& pos);
    Point getPosition() const;
    
    void setSize(const Size& size);
    Size getSize() const;
    
    void setX(int x);
    int getX() const;
    
    void setY(int y);
    int getY() const;
    
    void setWidth(int width);
    int getWidth() const;
    
    void setHeight(int height);
    int getHeight() const;
    
    // 显示状态
    void setVisible(bool visible);
    bool isVisible() const;
    
    void show() { setVisible(true); }
    void hide() { setVisible(false); }
    
    // 启用状态
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // 焦点
    void setFocus();
    bool hasFocus() const;
    void setFocusPolicy(FocusPolicy policy);
    FocusPolicy getFocusPolicy() const;
    
    // 样式属性
    void setBackgroundColor(const Color& color);
    Color getBackgroundColor() const;
    
    void setBorderColor(const Color& color);
    Color getBorderColor() const;
    
    void setBorderWidth(int width);
    int getBorderWidth() const;
    
    void setBorderRadius(int radius);
    int getBorderRadius() const;
    
    void setOpacity(float opacity);
    float getOpacity() const;
    
    void setMargin(int margin);
    void setMargins(int left, int top, int right, int bottom);
    int getMargin() const;
    
    void setPadding(int padding);
    void setPaddings(int left, int top, int right, int bottom);
    int getPadding() const;
    
    // 布局系统
    void setLayout(Layout* layout);
    Layout* getLayout() const;
    
    // 父子控件管理
    Widget* parentWidget() const;
    const std::vector<Widget*>& childWidgets() const;
    
    // 坐标转换
    Point mapToGlobal(const Point& pos) const;
    Point mapFromGlobal(const Point& pos) const;
    Point mapToParent(const Point& pos) const;
    Point mapFromParent(const Point& pos) const;
    
    // 更新和重绘
    void update();
    void repaint();
    
    // 样式表支持
    void setStyleSheet(const String& css);
    String getStyleSheet() const;
    
    // LVGL 对象访问
    lv_obj_t* lvglObject() const { return m_lvglObj; }
    
    // 信号
    SIGNAL(geometryChanged, const Rect&);
    SIGNAL(visibilityChanged, bool);
    SIGNAL(enabledChanged, bool);
    SIGNAL(focusChanged, bool);
    
protected:
    // 子类可重写的事件处理
    virtual void paintEvent() {}
    virtual void resizeEvent(const Size& newSize, const Size& oldSize) {}
    virtual void moveEvent(const Point& newPos, const Point& oldPos) {}
    virtual void showEvent() {}
    virtual void hideEvent() {}
    virtual void focusInEvent() {}
    virtual void focusOutEvent() {}
    virtual void mouseClickEvent(const Point& pos) {}
    virtual void mousePressEvent(const Point& pos) {}
    virtual void mouseReleaseEvent(const Point& pos) {}
    virtual void mouseMoveEvent(const Point& pos) {}
    
    // LVGL 事件回调
    static void lvglEventCallback(lv_event_t* e);
    
    // 创建 LVGL 对象（子类可重写）
    virtual lv_obj_t* createLvglObject(lv_obj_t* parent);
    
    // 应用样式到 LVGL 对象
    void applyStylesToLvgl();
    
private:
    lv_obj_t* m_lvglObj = nullptr;
    Layout* m_layout = nullptr;
    std::vector<Widget*> m_childWidgets;
    String m_styleSheet;
    FocusPolicy m_focusPolicy = FocusPolicy::NoFocus;
    
    void initializeLvglObject();
    void updateChildWidgets();
    void handleLvglEvent(lv_event_code_t eventCode);
};

} // namespace QGL

#endif // QGL_WIDGETS_WIDGET_H