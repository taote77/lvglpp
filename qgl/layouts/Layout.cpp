#include "Layout.h"
#include "../widgets/Widget.h"
#include <algorithm>

namespace QGL {

// WidgetItem 实现
WidgetItem::WidgetItem(Widget* widget) : m_widget(widget) {}

Size WidgetItem::sizeHint() const {
    return m_widget ? m_widget->getSize() : Size();
}

Size WidgetItem::minimumSize() const {
    return m_widget ? Size(0, 0) : Size();
}

Size WidgetItem::maximumSize() const {
    return Size(16777215, 16777215); // 最大尺寸
}

void WidgetItem::setGeometry(const Rect& rect) {
    if (m_widget) {
        m_widget->setGeometry(rect);
    }
}

Rect WidgetItem::geometry() const {
    return m_widget ? m_widget->getGeometry() : Rect();
}

bool WidgetItem::isEmpty() const {
    return !m_widget || !m_widget->isVisible();
}

// SpacerItem 实现
SpacerItem::SpacerItem(int width, int height, int hStretch, int vStretch)
    : m_size(width, height), m_hStretch(hStretch), m_vStretch(vStretch) {}

Size SpacerItem::sizeHint() const {
    return m_size;
}

Size SpacerItem::minimumSize() const {
    return Size(0, 0);
}

Size SpacerItem::maximumSize() const {
    return Size(16777215, 16777215);
}

void SpacerItem::setGeometry(const Rect& rect) {
    m_geometry = rect;
}

Rect SpacerItem::geometry() const {
    return m_geometry;
}

bool SpacerItem::isEmpty() const {
    return false;
}

// Layout 实现
Layout::Layout(Widget* parent) : Object(parent), m_parentWidget(parent) {}

Layout::~Layout() {
    // 清理所有布局项
    while (count() > 0) {
        delete takeAt(0);
    }
}

void Layout::addWidget(Widget* widget) {
    if (widget) {
        addItem(new WidgetItem(widget));
    }
}

void Layout::removeWidget(Widget* widget) {
    if (!widget) return;
    
    for (int i = 0; i < count(); ++i) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(itemAt(i))) {
            if (widgetItem->widget() == widget) {
                delete takeAt(i);
                break;
            }
        }
    }
}

int Layout::indexOf(Widget* widget) const {
    if (!widget) return -1;
    
    for (int i = 0; i < count(); ++i) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(itemAt(i))) {
            if (widgetItem->widget() == widget) {
                return i;
            }
        }
    }
    return -1;
}

void Layout::addSpacing(int size) {
    addItem(new SpacerItem(size, size));
}

void Layout::addStretch(int stretch) {
    addItem(new SpacerItem(0, 0, stretch, stretch));
}

void Layout::setSpacing(int spacing) {
    if (m_spacing != spacing) {
        m_spacing = spacing;
        invalidate();
    }
}

void Layout::setMargins(int left, int top, int right, int bottom) {
    Rect newMargins(left, top, right, bottom);
    if (m_margins.x != newMargins.x || m_margins.y != newMargins.y ||
        m_margins.width != newMargins.width || m_margins.height != newMargins.height) {
        m_margins = newMargins;
        invalidate();
    }
}

Rect Layout::getContentsRect() const {
    if (!m_parentWidget) {
        return Rect();
    }
    
    Rect parentRect = m_parentWidget->getGeometry();
    return Rect(
        parentRect.x + m_margins.x,
        parentRect.y + m_margins.y,
        parentRect.width - m_margins.x - m_margins.width,
        parentRect.height - m_margins.y - m_margins.height
    );
}

Size Layout::sizeHint() const {
    return calculateSizeHint();
}

Size Layout::minimumSize() const {
    return calculateMinimumSize();
}

Size Layout::maximumSize() const {
    return calculateMaximumSize();
}

void Layout::setParentWidget(Widget* widget) {
    m_parentWidget = widget;
    invalidate();
}

void Layout::activate() {
    if (m_parentWidget && m_enabled) {
        ensureGeometry();
    }
}

void Layout::update() {
    activate();
}

void Layout::invalidate() {
    markDirty();
    if (m_parentWidget) {
        m_parentWidget->update();
    }
}

void Layout::setEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (enabled) {
            activate();
        }
    }
}

Rect Layout::alignmentRect(const Rect& rect, const Size& size, Alignment alignment) const {
    int x = rect.x;
    int y = rect.y;
    int w = size.width;
    int h = size.height;
    
    // 水平对齐
    if (static_cast<int>(alignment) & static_cast<int>(Alignment::Right)) {
        x = rect.x + rect.width - w;
    } else if (static_cast<int>(alignment) & static_cast<int>(Alignment::HCenter)) {
        x = rect.x + (rect.width - w) / 2;
    }
    
    // 垂直对齐
    if (static_cast<int>(alignment) & static_cast<int>(Alignment::Bottom)) {
        y = rect.y + rect.height - h;
    } else if (static_cast<int>(alignment) & static_cast<int>(Alignment::VCenter)) {
        y = rect.y + (rect.height - h) / 2;
    }
    
    return Rect(x, y, w, h);
}

LayoutItem* Layout::findItemForWidget(Widget* widget) const {
    for (int i = 0; i < count(); ++i) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(itemAt(i))) {
            if (widgetItem->widget() == widget) {
                return widgetItem;
            }
        }
    }
    return nullptr;
}

void Layout::ensureGeometry() {
    if (m_dirty && m_parentWidget && m_enabled) {
        setGeometry(getContentsRect());
        m_dirty = false;
    }
}

// LayoutUtils 实现
namespace LayoutUtils {

Size expandSize(const Size& size, const Size& hint) {
    return Size(
        std::max(size.width, hint.width),
        std::max(size.height, hint.height)
    );
}

Size shrinkSize(const Size& size, const Size& hint) {
    return Size(
        std::min(size.width, hint.width),
        std::min(size.height, hint.height)
    );
}

int distribute(int available, const std::vector<int>& minimums, 
               const std::vector<int>& stretches) {
    if (minimums.empty()) return 0;
    
    int totalMinimum = 0;
    int totalStretch = 0;
    
    for (size_t i = 0; i < minimums.size(); ++i) {
        totalMinimum += minimums[i];
        if (i < stretches.size()) {
            totalStretch += stretches[i];
        }
    }
    
    if (available <= totalMinimum) {
        return totalMinimum;
    }
    
    int extra = available - totalMinimum;
    if (totalStretch == 0) {
        // 平均分配额外空间
        return available;
    }
    
    // 按比例分配额外空间
    return available;
}

} // namespace LayoutUtils

} // namespace QGL