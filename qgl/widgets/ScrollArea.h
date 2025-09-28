#pragma once

#include "Widget.h"
#include "../core/Types.h"
#include "../signals/Signal.h"
#include "../properties/Property.h"

namespace QGL {

/**
 * @brief 滚动条策略
 */
enum class ScrollBarPolicy {
    ScrollBarAsNeeded,  // 需要时显示
    ScrollBarAlwaysOff, // 总是隐藏
    ScrollBarAlwaysOn   // 总是显示
};

/**
 * @brief 滚动方向
 */
enum class ScrollDirection {
    None = 0x00,
    Horizontal = 0x01,
    Vertical = 0x02,
    Both = Horizontal | Vertical
};

/**
 * @brief 滚动区域控件
 * 
 * ScrollArea提供了一个可滚动的视图区域，可以包含比其可见区域更大的内容。
 * 支持水平和垂直滚动，滚动条策略配置，以及平滑滚动动画。
 */
class ScrollArea : public Widget {
    QGL_OBJECT(ScrollArea)

public:
    /**
     * @brief 构造函数
     * @param parent 父控件
     */
    explicit ScrollArea(Widget* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ScrollArea() override;

    // 内容控件管理
    void setWidget(Widget* widget);
    Widget* widget() const;
    Widget* takeWidget();
    
    // 滚动条策略
    void setHorizontalScrollBarPolicy(ScrollBarPolicy policy);
    ScrollBarPolicy horizontalScrollBarPolicy() const;
    
    void setVerticalScrollBarPolicy(ScrollBarPolicy policy);
    ScrollBarPolicy verticalScrollBarPolicy() const;
    
    // 滚动位置
    void setHorizontalScrollValue(int value);
    int horizontalScrollValue() const;
    
    void setVerticalScrollValue(int value);
    int verticalScrollValue() const;
    
    void setScrollPosition(const Point& position);
    Point scrollPosition() const;
    
    // 滚动范围
    int horizontalScrollMaximum() const;
    int verticalScrollMaximum() const;
    
    int horizontalScrollMinimum() const;
    int verticalScrollMinimum() const;
    
    // 视口大小
    Size viewportSize() const;
    Rect viewportRect() const;
    
    // 内容大小
    Size contentSize() const;
    void setContentSize(const Size& size);
    
    // 滚动步长
    void setHorizontalScrollStep(int step);
    int horizontalScrollStep() const;
    
    void setVerticalScrollStep(int step);
    int verticalScrollStep() const;
    
    // 页面步长
    void setHorizontalPageStep(int step);
    int horizontalPageStep() const;
    
    void setVerticalPageStep(int step);
    int verticalPageStep() const;
    
    // 滚动操作
    void scrollToTop();
    void scrollToBottom();
    void scrollToLeft();
    void scrollToRight();
    void scrollToPosition(const Point& position);
    void scrollBy(int dx, int dy);
    
    // 确保区域可见
    void ensureVisible(const Rect& rect, int xMargin = 50, int yMargin = 50);
    void ensureWidgetVisible(Widget* widget, int xMargin = 50, int yMargin = 50);
    
    // 滚动动画
    void setScrollAnimationEnabled(bool enabled);
    bool isScrollAnimationEnabled() const;
    
    void setScrollAnimationDuration(int duration);
    int scrollAnimationDuration() const;
    
    // 弹性滚动
    void setElasticScrollingEnabled(bool enabled);
    bool isElasticScrollingEnabled() const;
    
    // 滚动惯性
    void setScrollInertiaEnabled(bool enabled);
    bool isScrollInertiaEnabled() const;
    
    // 缩放支持
    void setZoomEnabled(bool enabled);
    bool isZoomEnabled() const;
    
    void setZoomFactor(double factor);
    double zoomFactor() const;
    
    void setZoomRange(double minFactor, double maxFactor);
    double minZoomFactor() const;
    double maxZoomFactor() const;
    
    // 边框和边距
    void setFrameStyle(int style);
    int frameStyle() const;
    
    void setViewportMargins(int left, int top, int right, int bottom);
    void setViewportMargins(const Rect& margins);
    Rect viewportMargins() const;
    
    // 背景和前景
    void setViewportBackgroundColor(const Color& color);
    Color viewportBackgroundColor() const;
    
    // 坐标转换
    Point mapToViewport(const Point& point) const;
    Point mapFromViewport(const Point& point) const;
    Rect mapToViewport(const Rect& rect) const;
    Rect mapFromViewport(const Rect& rect) const;
    
    // 滚动条访问
    Widget* horizontalScrollBar() const;
    Widget* verticalScrollBar() const;
    
    // 角落控件
    void setCornerWidget(Widget* widget);
    Widget* cornerWidget() const;

    // 信号
    SIGNAL(horizontalScrollValueChanged, int);       // 水平滚动值改变
    SIGNAL(verticalScrollValueChanged, int);         // 垂直滚动值改变
    SIGNAL(scrollPositionChanged, const Point&);     // 滚动位置改变
    SIGNAL(contentSizeChanged, const Size&);         // 内容大小改变
    SIGNAL(viewportSizeChanged, const Size&);        // 视口大小改变
    SIGNAL(zoomFactorChanged, double);               // 缩放因子改变

protected:
    // 事件处理
    void resizeEvent(ResizeEvent* event) override;
    void wheelEvent(WheelEvent* event) override;
    void mousePressEvent(MouseEvent* event) override;
    void mouseMoveEvent(MouseEvent* event) override;
    void mouseReleaseEvent(MouseEvent* event) override;
    void keyPressEvent(KeyEvent* event) override;
    
    // 滚动事件
    virtual void scrollContentsBy(int dx, int dy);
    virtual void updateScrollBars();
    virtual void updateViewport();
    
    // 子类可重写的视口绘制
    virtual void paintViewport(PaintEvent* event);
    
    // 内部方法
    void setupScrollBars();
    void updateContentGeometry();
    bool isScrollBarVisible(ScrollBarPolicy policy, bool needed) const;
    
    // 动画相关
    void startScrollAnimation(const Point& targetPosition);
    void stopScrollAnimation();

private:
    class ScrollAreaPrivate;
    std::unique_ptr<ScrollAreaPrivate> d_ptr;
    
    void initializeScrollArea();
    void setupLVGLScrollView();
    void connectLVGLSignals();
    
    // LVGL 回调函数
    static void onScrollValueChanged(lv_event_t* e);
    static void onScrollEnd(lv_event_t* e);
    static void onScrollBegin(lv_event_t* e);
    
    // 属性声明
    QGL_PROPERTY(ScrollBarPolicy, horizontalScrollBarPolicy, HORIZONTAL_SCROLLBAR_POLICY)
    QGL_PROPERTY(ScrollBarPolicy, verticalScrollBarPolicy, VERTICAL_SCROLLBAR_POLICY)
    QGL_PROPERTY(int, horizontalScrollValue, HORIZONTAL_SCROLL_VALUE)
    QGL_PROPERTY(int, verticalScrollValue, VERTICAL_SCROLL_VALUE)
    QGL_PROPERTY(Size, contentSize, CONTENT_SIZE)
    QGL_PROPERTY(int, horizontalScrollStep, HORIZONTAL_SCROLL_STEP)
    QGL_PROPERTY(int, verticalScrollStep, VERTICAL_SCROLL_STEP)
    QGL_PROPERTY(int, horizontalPageStep, HORIZONTAL_PAGE_STEP)
    QGL_PROPERTY(int, verticalPageStep, VERTICAL_PAGE_STEP)
    QGL_PROPERTY(bool, scrollAnimationEnabled, SCROLL_ANIMATION_ENABLED)
    QGL_PROPERTY(int, scrollAnimationDuration, SCROLL_ANIMATION_DURATION)
    QGL_PROPERTY(bool, elasticScrollingEnabled, ELASTIC_SCROLLING_ENABLED)
    QGL_PROPERTY(bool, scrollInertiaEnabled, SCROLL_INERTIA_ENABLED)
    QGL_PROPERTY(bool, zoomEnabled, ZOOM_ENABLED)
    QGL_PROPERTY(double, zoomFactor, ZOOM_FACTOR)
    QGL_PROPERTY(double, minZoomFactor, MIN_ZOOM_FACTOR)
    QGL_PROPERTY(double, maxZoomFactor, MAX_ZOOM_FACTOR)
    QGL_PROPERTY(int, frameStyle, FRAME_STYLE)
    QGL_PROPERTY(Color, viewportBackgroundColor, VIEWPORT_BACKGROUND_COLOR)
};

} // namespace QGL