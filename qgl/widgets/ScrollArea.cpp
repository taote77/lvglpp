#include "ScrollArea.h"
#include "../core/Application.h"
#include "../events/ResizeEvent.h"
#include "../events/WheelEvent.h"
#include "../events/MouseEvent.h"
#include "../events/KeyEvent.h"
#include "../events/PaintEvent.h"
#include <lvgl.h>
#include <algorithm>

namespace QGL {

// ScrollArea私有数据类
class ScrollArea::ScrollAreaPrivate {
public:
    Widget* contentWidget = nullptr;
    Widget* horizontalScrollBar = nullptr;
    Widget* verticalScrollBar = nullptr;
    Widget* cornerWidget = nullptr;
    
    ScrollBarPolicy horizontalPolicy = ScrollBarPolicy::ScrollBarAsNeeded;
    ScrollBarPolicy verticalPolicy = ScrollBarPolicy::ScrollBarAsNeeded;
    
    int horizontalValue = 0;
    int verticalValue = 0;
    int horizontalMin = 0;
    int horizontalMax = 0;
    int verticalMin = 0;
    int verticalMax = 0;
    
    int horizontalStep = 1;
    int verticalStep = 1;
    int horizontalPageStep = 10;
    int verticalPageStep = 10;
    
    Size contentSize;
    Size viewportSize;
    Rect viewportMargins = {0, 0, 0, 0};
    
    bool scrollAnimationEnabled = true;
    int scrollAnimationDuration = 250;
    bool elasticScrollingEnabled = false;
    bool scrollInertiaEnabled = true;
    
    bool zoomEnabled = false;
    double zoomFactor = 1.0;
    double minZoomFactor = 0.1;
    double maxZoomFactor = 10.0;
    
    int frameStyle = 0;
    Color viewportBackgroundColor = Color::White;
    
    // LVGL相关
    lv_obj_t* lvgl_scrollview = nullptr;
    lv_obj_t* lvgl_content = nullptr;
    
    // 内部状态
    bool updating = false;
    Point lastMousePos;
    bool isDragging = false;
    
    ScrollAreaPrivate() = default;
};

ScrollArea::ScrollArea(Widget* parent)
    : Widget(parent)
    , d_ptr(std::make_unique<ScrollAreaPrivate>()) {
    initializeScrollArea();
}

ScrollArea::~ScrollArea() {
    if (d_ptr->contentWidget) {
        delete d_ptr->contentWidget;
    }
    
    if (d_ptr->lvgl_scrollview) {
        lv_obj_del(d_ptr->lvgl_scrollview);
    }
}

void ScrollArea::initializeScrollArea() {
    setObjectName("ScrollArea");
    setupLVGLScrollView();
    connectLVGLSignals();
    
    // 设置默认样式
    setProperty("background-color", Color(245, 245, 245));
    setProperty("border", "1px solid #cccccc");
    setProperty("border-radius", "4px");
}

void ScrollArea::setupLVGLScrollView() {
    // 创建LVGL滚动视图
    d_ptr->lvgl_scrollview = lv_obj_create(getLVGLObject());
    lv_obj_set_size(d_ptr->lvgl_scrollview, LV_PCT(100), LV_PCT(100));
    lv_obj_align(d_ptr->lvgl_scrollview, LV_ALIGN_CENTER, 0, 0);
    
    // 启用滚动
    lv_obj_set_scroll_dir(d_ptr->lvgl_scrollview, LV_DIR_ALL);
    lv_obj_set_scroll_snap_x(d_ptr->lvgl_scrollview, LV_SCROLL_SNAP_NONE);
    lv_obj_set_scroll_snap_y(d_ptr->lvgl_scrollview, LV_SCROLL_SNAP_NONE);
    
    // 创建内容容器
    d_ptr->lvgl_content = lv_obj_create(d_ptr->lvgl_scrollview);
    lv_obj_set_size(d_ptr->lvgl_content, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(d_ptr->lvgl_content, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // 设置样式
    lv_obj_set_style_bg_color(d_ptr->lvgl_scrollview, lv_color_hex(0xf5f5f5), 0);
    lv_obj_set_style_border_color(d_ptr->lvgl_scrollview, lv_color_hex(0xcccccc), 0);
    lv_obj_set_style_border_width(d_ptr->lvgl_scrollview, 1, 0);
    lv_obj_set_style_radius(d_ptr->lvgl_scrollview, 4, 0);
    lv_obj_set_style_pad_all(d_ptr->lvgl_scrollview, 0, 0);
}

void ScrollArea::connectLVGLSignals() {
    lv_obj_add_event_cb(d_ptr->lvgl_scrollview, onScrollValueChanged, LV_EVENT_SCROLL, this);
    lv_obj_add_event_cb(d_ptr->lvgl_scrollview, onScrollBegin, LV_EVENT_SCROLL_BEGIN, this);
    lv_obj_add_event_cb(d_ptr->lvgl_scrollview, onScrollEnd, LV_EVENT_SCROLL_END, this);
}

// 内容控件管理
void ScrollArea::setWidget(Widget* widget) {
    if (d_ptr->contentWidget == widget) {
        return;
    }
    
    // 移除旧的内容控件
    if (d_ptr->contentWidget) {
        // 这里应该从LVGL容器中移除
        delete d_ptr->contentWidget;
    }
    
    d_ptr->contentWidget = widget;
    
    if (widget) {
        // 将新控件添加到内容容器中
        if (widget->getLVGLObject() && d_ptr->lvgl_content) {
            lv_obj_set_parent(widget->getLVGLObject(), d_ptr->lvgl_content);
        }
        
        // 更新内容大小
        updateContentGeometry();
    }
}

Widget* ScrollArea::widget() const {
    return d_ptr->contentWidget;
}

Widget* ScrollArea::takeWidget() {
    Widget* widget = d_ptr->contentWidget;
    d_ptr->contentWidget = nullptr;
    
    if (widget && widget->getLVGLObject()) {
        // 将控件从内容容器中移除
        lv_obj_set_parent(widget->getLVGLObject(), nullptr);
    }
    
    return widget;
}

// 滚动条策略
void ScrollArea::setHorizontalScrollBarPolicy(ScrollBarPolicy policy) {
    d_ptr->horizontalPolicy = policy;
    updateScrollBars();
}

ScrollBarPolicy ScrollArea::horizontalScrollBarPolicy() const {
    return d_ptr->horizontalPolicy;
}

void ScrollArea::setVerticalScrollBarPolicy(ScrollBarPolicy policy) {
    d_ptr->verticalPolicy = policy;
    updateScrollBars();
}

ScrollBarPolicy ScrollArea::verticalScrollBarPolicy() const {
    return d_ptr->verticalPolicy;
}

// 滚动位置
void ScrollArea::setHorizontalScrollValue(int value) {
    value = std::max(d_ptr->horizontalMin, std::min(value, d_ptr->horizontalMax));
    
    if (d_ptr->horizontalValue != value) {
        d_ptr->horizontalValue = value;
        
        if (d_ptr->lvgl_scrollview && !d_ptr->updating) {
            d_ptr->updating = true;
            lv_obj_scroll_to_x(d_ptr->lvgl_scrollview, value, d_ptr->scrollAnimationEnabled ? LV_ANIM_ON : LV_ANIM_OFF);
            d_ptr->updating = false;
        }
        
        horizontalScrollValueChanged.emit(value);
        scrollPositionChanged.emit(Point(d_ptr->horizontalValue, d_ptr->verticalValue));
    }
}

int ScrollArea::horizontalScrollValue() const {
    return d_ptr->horizontalValue;
}

void ScrollArea::setVerticalScrollValue(int value) {
    value = std::max(d_ptr->verticalMin, std::min(value, d_ptr->verticalMax));
    
    if (d_ptr->verticalValue != value) {
        d_ptr->verticalValue = value;
        
        if (d_ptr->lvgl_scrollview && !d_ptr->updating) {
            d_ptr->updating = true;
            lv_obj_scroll_to_y(d_ptr->lvgl_scrollview, value, d_ptr->scrollAnimationEnabled ? LV_ANIM_ON : LV_ANIM_OFF);
            d_ptr->updating = false;
        }
        
        verticalScrollValueChanged.emit(value);
        scrollPositionChanged.emit(Point(d_ptr->horizontalValue, d_ptr->verticalValue));
    }
}

int ScrollArea::verticalScrollValue() const {
    return d_ptr->verticalValue;
}

void ScrollArea::setScrollPosition(const Point& position) {
    setHorizontalScrollValue(position.x);
    setVerticalScrollValue(position.y);
}

Point ScrollArea::scrollPosition() const {
    return Point(d_ptr->horizontalValue, d_ptr->verticalValue);
}

// 滚动范围
int ScrollArea::horizontalScrollMaximum() const {
    return d_ptr->horizontalMax;
}

int ScrollArea::verticalScrollMaximum() const {
    return d_ptr->verticalMax;
}

int ScrollArea::horizontalScrollMinimum() const {
    return d_ptr->horizontalMin;
}

int ScrollArea::verticalScrollMinimum() const {
    return d_ptr->verticalMin;
}

// 视口大小
Size ScrollArea::viewportSize() const {
    return d_ptr->viewportSize;
}

Rect ScrollArea::viewportRect() const {
    return Rect(Point(0, 0), d_ptr->viewportSize);
}

// 内容大小
Size ScrollArea::contentSize() const {
    return d_ptr->contentSize;
}

void ScrollArea::setContentSize(const Size& size) {
    if (d_ptr->contentSize != size) {
        d_ptr->contentSize = size;
        
        if (d_ptr->lvgl_content) {
            lv_obj_set_size(d_ptr->lvgl_content, size.width, size.height);
        }
        
        updateScrollBars();
        contentSizeChanged.emit(size);
    }
}

// 滚动步长
void ScrollArea::setHorizontalScrollStep(int step) {
    d_ptr->horizontalStep = std::max(1, step);
}

int ScrollArea::horizontalScrollStep() const {
    return d_ptr->horizontalStep;
}

void ScrollArea::setVerticalScrollStep(int step) {
    d_ptr->verticalStep = std::max(1, step);
}

int ScrollArea::verticalScrollStep() const {
    return d_ptr->verticalStep;
}

// 页面步长
void ScrollArea::setHorizontalPageStep(int step) {
    d_ptr->horizontalPageStep = std::max(1, step);
}

int ScrollArea::horizontalPageStep() const {
    return d_ptr->horizontalPageStep;
}

void ScrollArea::setVerticalPageStep(int step) {
    d_ptr->verticalPageStep = std::max(1, step);
}

int ScrollArea::verticalPageStep() const {
    return d_ptr->verticalPageStep;
}

// 滚动操作
void ScrollArea::scrollToTop() {
    setVerticalScrollValue(d_ptr->verticalMin);
}

void ScrollArea::scrollToBottom() {
    setVerticalScrollValue(d_ptr->verticalMax);
}

void ScrollArea::scrollToLeft() {
    setHorizontalScrollValue(d_ptr->horizontalMin);
}

void ScrollArea::scrollToRight() {
    setHorizontalScrollValue(d_ptr->horizontalMax);
}

void ScrollArea::scrollToPosition(const Point& position) {
    setScrollPosition(position);
}

void ScrollArea::scrollBy(int dx, int dy) {
    setHorizontalScrollValue(d_ptr->horizontalValue + dx);
    setVerticalScrollValue(d_ptr->verticalValue + dy);
}

// 确保区域可见
void ScrollArea::ensureVisible(const Rect& rect, int xMargin, int yMargin) {
    int hValue = d_ptr->horizontalValue;
    int vValue = d_ptr->verticalValue;
    
    // 水平方向
    if (rect.x - xMargin < hValue) {
        hValue = rect.x - xMargin;
    } else if (rect.x + rect.width + xMargin > hValue + d_ptr->viewportSize.width) {
        hValue = rect.x + rect.width + xMargin - d_ptr->viewportSize.width;
    }
    
    // 垂直方向
    if (rect.y - yMargin < vValue) {
        vValue = rect.y - yMargin;
    } else if (rect.y + rect.height + yMargin > vValue + d_ptr->viewportSize.height) {
        vValue = rect.y + rect.height + yMargin - d_ptr->viewportSize.height;
    }
    
    setScrollPosition(Point(hValue, vValue));
}

void ScrollArea::ensureWidgetVisible(Widget* widget, int xMargin, int yMargin) {
    if (!widget) {
        return;
    }
    
    Rect widgetRect = widget->getGeometry();
    ensureVisible(widgetRect, xMargin, yMargin);
}

// 滚动动画
void ScrollArea::setScrollAnimationEnabled(bool enabled) {
    d_ptr->scrollAnimationEnabled = enabled;
}

bool ScrollArea::isScrollAnimationEnabled() const {
    return d_ptr->scrollAnimationEnabled;
}

void ScrollArea::setScrollAnimationDuration(int duration) {
    d_ptr->scrollAnimationDuration = std::max(0, duration);
}

int ScrollArea::scrollAnimationDuration() const {
    return d_ptr->scrollAnimationDuration;
}

// 弹性滚动
void ScrollArea::setElasticScrollingEnabled(bool enabled) {
    d_ptr->elasticScrollingEnabled = enabled;
}

bool ScrollArea::isElasticScrollingEnabled() const {
    return d_ptr->elasticScrollingEnabled;
}

// 滚动惯性
void ScrollArea::setScrollInertiaEnabled(bool enabled) {
    d_ptr->scrollInertiaEnabled = enabled;
}

bool ScrollArea::isScrollInertiaEnabled() const {
    return d_ptr->scrollInertiaEnabled;
}

// 缩放支持
void ScrollArea::setZoomEnabled(bool enabled) {
    d_ptr->zoomEnabled = enabled;
}

bool ScrollArea::isZoomEnabled() const {
    return d_ptr->zoomEnabled;
}

void ScrollArea::setZoomFactor(double factor) {
    factor = std::max(d_ptr->minZoomFactor, std::min(factor, d_ptr->maxZoomFactor));
    
    if (d_ptr->zoomFactor != factor) {
        d_ptr->zoomFactor = factor;
        
        // 更新内容大小
        updateContentGeometry();
        
        zoomFactorChanged.emit(factor);
    }
}

double ScrollArea::zoomFactor() const {
    return d_ptr->zoomFactor;
}

void ScrollArea::setZoomRange(double minFactor, double maxFactor) {
    d_ptr->minZoomFactor = std::max(0.01, minFactor);
    d_ptr->maxZoomFactor = std::max(d_ptr->minZoomFactor, maxFactor);
    
    // 确保当前缩放因子在范围内
    setZoomFactor(d_ptr->zoomFactor);
}

double ScrollArea::minZoomFactor() const {
    return d_ptr->minZoomFactor;
}

double ScrollArea::maxZoomFactor() const {
    return d_ptr->maxZoomFactor;
}

// 边框和边距
void ScrollArea::setFrameStyle(int style) {
    d_ptr->frameStyle = style;
    // TODO: 应用边框样式到LVGL对象
}

int ScrollArea::frameStyle() const {
    return d_ptr->frameStyle;
}

void ScrollArea::setViewportMargins(int left, int top, int right, int bottom) {
    d_ptr->viewportMargins = Rect(left, top, right - left, bottom - top);
    updateViewport();
}

void ScrollArea::setViewportMargins(const Rect& margins) {
    d_ptr->viewportMargins = margins;
    updateViewport();
}

Rect ScrollArea::viewportMargins() const {
    return d_ptr->viewportMargins;
}

// 背景和前景
void ScrollArea::setViewportBackgroundColor(const Color& color) {
    d_ptr->viewportBackgroundColor = color;
    
    if (d_ptr->lvgl_scrollview) {
        lv_color_t lvColor = lv_color_make(color.red(), color.green(), color.blue());
        lv_obj_set_style_bg_color(d_ptr->lvgl_scrollview, lvColor, 0);
    }
}

Color ScrollArea::viewportBackgroundColor() const {
    return d_ptr->viewportBackgroundColor;
}

// 坐标转换
Point ScrollArea::mapToViewport(const Point& point) const {
    return Point(point.x - d_ptr->horizontalValue, point.y - d_ptr->verticalValue);
}

Point ScrollArea::mapFromViewport(const Point& point) const {
    return Point(point.x + d_ptr->horizontalValue, point.y + d_ptr->verticalValue);
}

Rect ScrollArea::mapToViewport(const Rect& rect) const {
    return Rect(mapToViewport(rect.topLeft()), rect.size());
}

Rect ScrollArea::mapFromViewport(const Rect& rect) const {
    return Rect(mapFromViewport(rect.topLeft()), rect.size());
}

// 滚动条访问
Widget* ScrollArea::horizontalScrollBar() const {
    return d_ptr->horizontalScrollBar;
}

Widget* ScrollArea::verticalScrollBar() const {
    return d_ptr->verticalScrollBar;
}

// 角落控件
void ScrollArea::setCornerWidget(Widget* widget) {
    if (d_ptr->cornerWidget) {
        delete d_ptr->cornerWidget;
    }
    
    d_ptr->cornerWidget = widget;
    // TODO: 设置角落控件的位置
}

Widget* ScrollArea::cornerWidget() const {
    return d_ptr->cornerWidget;
}

// 事件处理
void ScrollArea::resizeEvent(ResizeEvent* event) {
    Widget::resizeEvent(event);
    
    d_ptr->viewportSize = event->size();
    updateScrollBars();
    updateViewport();
    
    viewportSizeChanged.emit(d_ptr->viewportSize);
}

void ScrollArea::wheelEvent(WheelEvent* event) {
    // 处理鼠标滚轮事件
    int delta = event->delta();
    
    if (event->modifiers() & KeyModifier::Shift) {
        // Shift + 滚轮 = 水平滚动
        scrollBy(delta, 0);
    } else {
        // 普通滚轮 = 垂直滚动
        scrollBy(0, delta);
    }
    
    event->accept();
}

void ScrollArea::mousePressEvent(MouseEvent* event) {
    d_ptr->lastMousePos = event->position();
    d_ptr->isDragging = true;
}

void ScrollArea::mouseMoveEvent(MouseEvent* event) {
    if (d_ptr->isDragging) {
        Point delta = d_ptr->lastMousePos - event->position();
        scrollBy(delta.x, delta.y);
        d_ptr->lastMousePos = event->position();
    }
}

void ScrollArea::mouseReleaseEvent(MouseEvent* event) {
    d_ptr->isDragging = false;
}

void ScrollArea::keyPressEvent(KeyEvent* event) {
    // 处理键盘滚动
    switch (event->key()) {
        case Key::Up:
            scrollBy(0, -d_ptr->verticalStep);
            break;
        case Key::Down:
            scrollBy(0, d_ptr->verticalStep);
            break;
        case Key::Left:
            scrollBy(-d_ptr->horizontalStep, 0);
            break;
        case Key::Right:
            scrollBy(d_ptr->horizontalStep, 0);
            break;
        case Key::PageUp:
            scrollBy(0, -d_ptr->verticalPageStep);
            break;
        case Key::PageDown:
            scrollBy(0, d_ptr->verticalPageStep);
            break;
        case Key::Home:
            scrollToTop();
            break;
        case Key::End:
            scrollToBottom();
            break;
        default:
            Widget::keyPressEvent(event);
            return;
    }
    
    event->accept();
}

// 内部方法
void ScrollArea::scrollContentsBy(int dx, int dy) {
    // 子类可重写此方法来自定义滚动行为
    if (d_ptr->contentWidget) {
        Point pos = d_ptr->contentWidget->getPosition();
        d_ptr->contentWidget->setPosition(Point(pos.x - dx, pos.y - dy));
    }
}

void ScrollArea::updateScrollBars() {
    if (!d_ptr->lvgl_scrollview) {
        return;
    }
    
    // 计算滚动范围
    d_ptr->horizontalMax = std::max(0, d_ptr->contentSize.width - d_ptr->viewportSize.width);
    d_ptr->verticalMax = std::max(0, d_ptr->contentSize.height - d_ptr->viewportSize.height);
    
    // 设置LVGL滚动条策略
    lv_scrollbar_mode_t hMode = LV_SCROLLBAR_MODE_AUTO;
    lv_scrollbar_mode_t vMode = LV_SCROLLBAR_MODE_AUTO;
    
    switch (d_ptr->horizontalPolicy) {
        case ScrollBarPolicy::ScrollBarAlwaysOff:
            hMode = LV_SCROLLBAR_MODE_OFF;
            break;
        case ScrollBarPolicy::ScrollBarAlwaysOn:
            hMode = LV_SCROLLBAR_MODE_ON;
            break;
        case ScrollBarPolicy::ScrollBarAsNeeded:
            hMode = LV_SCROLLBAR_MODE_AUTO;
            break;
    }
    
    switch (d_ptr->verticalPolicy) {
        case ScrollBarPolicy::ScrollBarAlwaysOff:
            vMode = LV_SCROLLBAR_MODE_OFF;
            break;
        case ScrollBarPolicy::ScrollBarAlwaysOn:
            vMode = LV_SCROLLBAR_MODE_ON;
            break;
        case ScrollBarPolicy::ScrollBarAsNeeded:
            vMode = LV_SCROLLBAR_MODE_AUTO;
            break;
    }
    
    lv_obj_set_scrollbar_mode(d_ptr->lvgl_scrollview, hMode);
    // LVGL没有单独设置垂直滚动条模式的API，使用相同的模式
}

void ScrollArea::updateViewport() {
    // 更新视口几何信息
    if (d_ptr->lvgl_scrollview) {
        lv_obj_set_style_pad_left(d_ptr->lvgl_scrollview, d_ptr->viewportMargins.x, 0);
        lv_obj_set_style_pad_top(d_ptr->lvgl_scrollview, d_ptr->viewportMargins.y, 0);
        lv_obj_set_style_pad_right(d_ptr->lvgl_scrollview, d_ptr->viewportMargins.width, 0);
        lv_obj_set_style_pad_bottom(d_ptr->lvgl_scrollview, d_ptr->viewportMargins.height, 0);
    }
}

void ScrollArea::paintViewport(PaintEvent* event) {
    // 子类可重写此方法来自定义视口绘制
}

void ScrollArea::setupScrollBars() {
    // 设置滚动条（简化实现）
}

void ScrollArea::updateContentGeometry() {
    if (d_ptr->contentWidget) {
        Size scaledSize = Size(
            static_cast<int>(d_ptr->contentSize.width * d_ptr->zoomFactor),
            static_cast<int>(d_ptr->contentSize.height * d_ptr->zoomFactor)
        );
        
        if (d_ptr->lvgl_content) {
            lv_obj_set_size(d_ptr->lvgl_content, scaledSize.width, scaledSize.height);
        }
        
        updateScrollBars();
    }
}

bool ScrollArea::isScrollBarVisible(ScrollBarPolicy policy, bool needed) const {
    switch (policy) {
        case ScrollBarPolicy::ScrollBarAlwaysOff:
            return false;
        case ScrollBarPolicy::ScrollBarAlwaysOn:
            return true;
        case ScrollBarPolicy::ScrollBarAsNeeded:
            return needed;
        default:
            return false;
    }
}

void ScrollArea::startScrollAnimation(const Point& targetPosition) {
    if (d_ptr->scrollAnimationEnabled && d_ptr->lvgl_scrollview) {
        lv_obj_scroll_to(d_ptr->lvgl_scrollview, targetPosition.x, targetPosition.y, LV_ANIM_ON);
    }
}

void ScrollArea::stopScrollAnimation() {
    if (d_ptr->lvgl_scrollview) {
        lv_anim_del(d_ptr->lvgl_scrollview, nullptr);
    }
}

// LVGL回调函数
void ScrollArea::onScrollValueChanged(lv_event_t* e) {
    ScrollArea* scrollArea = static_cast<ScrollArea*>(lv_event_get_user_data(e));
    
    if (scrollArea && !scrollArea->d_ptr->updating) {
        lv_obj_t* obj = lv_event_get_target(e);
        lv_coord_t x = lv_obj_get_scroll_x(obj);
        lv_coord_t y = lv_obj_get_scroll_y(obj);
        
        scrollArea->d_ptr->updating = true;
        scrollArea->d_ptr->horizontalValue = x;
        scrollArea->d_ptr->verticalValue = y;
        scrollArea->d_ptr->updating = false;
        
        scrollArea->horizontalScrollValueChanged.emit(x);
        scrollArea->verticalScrollValueChanged.emit(y);
        scrollArea->scrollPositionChanged.emit(Point(x, y));
    }
}

void ScrollArea::onScrollEnd(lv_event_t* e) {
    ScrollArea* scrollArea = static_cast<ScrollArea*>(lv_event_get_user_data(e));
    if (scrollArea) {
        // 滚动结束事件处理
    }
}

void ScrollArea::onScrollBegin(lv_event_t* e) {
    ScrollArea* scrollArea = static_cast<ScrollArea*>(lv_event_get_user_data(e));
    if (scrollArea) {
        // 滚动开始事件处理
    }
}

} // namespace QGL