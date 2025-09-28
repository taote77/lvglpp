#include "BoxLayout.h"
#include "../widgets/Widget.h"
#include <algorithm>

namespace QGL {

BoxLayout::BoxLayout(LayoutDirection direction, Widget* parent)
    : Layout(parent), m_direction(direction) {}

BoxLayout::~BoxLayout() {
    for (auto& item : m_items) {
        delete item.item;
    }
    m_items.clear();
}

void BoxLayout::addItem(LayoutItem* item) {
    if (item) {
        m_items.emplace_back(item);
        invalidate();
    }
}

int BoxLayout::count() const {
    return static_cast<int>(m_items.size());
}

LayoutItem* BoxLayout::itemAt(int index) const {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        return m_items[index].item;
    }
    return nullptr;
}

LayoutItem* BoxLayout::takeAt(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        LayoutItem* item = m_items[index].item;
        m_items.erase(m_items.begin() + index);
        invalidate();
        return item;
    }
    return nullptr;
}

void BoxLayout::insertItem(int index, LayoutItem* item) {
    if (!item) return;
    
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        addItem(item);
    } else {
        m_items.insert(m_items.begin() + index, BoxLayoutItem(item));
        invalidate();
    }
}

void BoxLayout::insertWidget(int index, Widget* widget) {
    if (widget) {
        insertItem(index, new WidgetItem(widget));
    }
}

void BoxLayout::insertSpacing(int index, int size) {
    insertItem(index, new SpacerItem(
        isHorizontal() ? size : 0,
        isHorizontal() ? 0 : size
    ));
}

void BoxLayout::insertStretch(int index, int stretch) {
    insertItem(index, new SpacerItem(0, 0, stretch, stretch));
}

void BoxLayout::setDirection(LayoutDirection direction) {
    if (m_direction != direction) {
        m_direction = direction;
        invalidate();
    }
}

void BoxLayout::setStretch(int index, int stretch) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_items[index].stretch = stretch;
        invalidate();
    }
}

int BoxLayout::getStretch(int index) const {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        return m_items[index].stretch;
    }
    return 0;
}

void BoxLayout::setAlignment(Widget* widget, Alignment alignment) {
    for (auto& item : m_items) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(item.item)) {
            if (widgetItem->widget() == widget) {
                item.alignment = alignment;
                invalidate();
                break;
            }
        }
    }
}

Alignment BoxLayout::getAlignment(Widget* widget) const {
    for (const auto& item : m_items) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(item.item)) {
            if (widgetItem->widget() == widget) {
                return item.alignment;
            }
        }
    }
    return Alignment::Left;
}

void BoxLayout::setGeometry(const Rect& rect) {
    if (m_items.empty()) return;
    
    distributeSpace(rect);
}

Size BoxLayout::calculateSizeHint() const {
    return calculateSize(false);
}

Size BoxLayout::calculateMinimumSize() const {
    return calculateSize(true);
}

Size BoxLayout::calculateMaximumSize() const {
    return Size(16777215, 16777215);
}

bool BoxLayout::isHorizontal() const {
    return m_direction == LayoutDirection::LeftToRight ||
           m_direction == LayoutDirection::RightToLeft;
}

void BoxLayout::distributeSpace(const Rect& rect) {
    if (m_items.empty()) return;
    
    const bool horizontal = isHorizontal();
    const int totalSpace = horizontal ? rect.width : rect.height;
    const int spacing = getSpacing();
    const int totalSpacing = (static_cast<int>(m_items.size()) - 1) * spacing;
    const int availableSpace = totalSpace - totalSpacing;
    
    // 计算每个项目的尺寸
    std::vector<int> sizes = calculateSizes(availableSpace);
    
    // 分配位置
    int pos = horizontal ? rect.x : rect.y;
    const bool reverse = (m_direction == LayoutDirection::RightToLeft ||
                         m_direction == LayoutDirection::BottomToTop);
    
    for (size_t i = 0; i < m_items.size(); ++i) {
        const size_t index = reverse ? (m_items.size() - 1 - i) : i;
        const int size = sizes[index];
        
        Rect itemRect;
        if (horizontal) {
            itemRect = Rect(pos, rect.y, size, rect.height);
        } else {
            itemRect = Rect(rect.x, pos, rect.width, size);
        }
        
        // 应用对齐
        const Size itemSizeHint = m_items[index].item->sizeHint();
        if (horizontal && itemSizeHint.height < rect.height) {
            // 垂直对齐
            itemRect = alignmentRect(itemRect, itemSizeHint, m_items[index].alignment);
        } else if (!horizontal && itemSizeHint.width < rect.width) {
            // 水平对齐
            itemRect = alignmentRect(itemRect, itemSizeHint, m_items[index].alignment);
        }
        
        m_items[index].item->setGeometry(itemRect);
        pos += size + spacing;
    }
}

Size BoxLayout::calculateSize(bool minimum) const {
    if (m_items.empty()) {
        return Size(0, 0);
    }
    
    const bool horizontal = isHorizontal();
    const int spacing = getSpacing();
    const int totalSpacing = (static_cast<int>(m_items.size()) - 1) * spacing;
    
    int mainAxis = 0;
    int crossAxis = 0;
    
    for (const auto& item : m_items) {
        if (item.item->isEmpty()) continue;
        
        Size itemSize = minimum ? item.item->minimumSize() : item.item->sizeHint();
        
        if (horizontal) {
            mainAxis += itemSize.width;
            crossAxis = std::max(crossAxis, itemSize.height);
        } else {
            mainAxis += itemSize.height;
            crossAxis = std::max(crossAxis, itemSize.width);
        }
    }
    
    mainAxis += totalSpacing;
    
    if (horizontal) {
        return Size(mainAxis, crossAxis);
    } else {
        return Size(crossAxis, mainAxis);
    }
}

int BoxLayout::totalStretch() const {
    int total = 0;
    for (const auto& item : m_items) {
        total += item.stretch;
        if (isHorizontal()) {
            total += item.item->horizontalStretch();
        } else {
            total += item.item->verticalStretch();
        }
    }
    return total;
}

std::vector<int> BoxLayout::calculateSizes(int available) const {
    std::vector<int> sizes;
    sizes.reserve(m_items.size());
    
    if (m_items.empty()) {
        return sizes;
    }
    
    // 首先分配最小尺寸
    int usedSpace = 0;
    for (const auto& item : m_items) {
        Size minSize = item.item->minimumSize();
        Size hintSize = item.item->sizeHint();
        
        int itemSize = isHorizontal() ? 
            std::max(minSize.width, hintSize.width) :
            std::max(minSize.height, hintSize.height);
            
        sizes.push_back(itemSize);
        usedSpace += itemSize;
    }
    
    // 分配剩余空间
    int remainingSpace = available - usedSpace;
    if (remainingSpace > 0) {
        int totalStretchFactor = 0;
        for (const auto& item : m_items) {
            totalStretchFactor += item.stretch;
        }
        
        if (totalStretchFactor > 0) {
            // 按拉伸因子分配
            for (size_t i = 0; i < m_items.size(); ++i) {
                int extraSpace = (remainingSpace * m_items[i].stretch) / totalStretchFactor;
                sizes[i] += extraSpace;
            }
        } else {
            // 平均分配
            int extraPerItem = remainingSpace / static_cast<int>(m_items.size());
            for (int& size : sizes) {
                size += extraPerItem;
            }
        }
    }
    
    return sizes;
}

} // namespace QGL