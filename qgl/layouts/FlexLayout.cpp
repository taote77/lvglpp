#include "FlexLayout.h"
#include "../widgets/Widget.h"
#include <algorithm>
#include <numeric>

namespace QGL {

FlexLayout::FlexLayout(FlexDirection direction, Widget* parent)
    : Layout(parent), m_direction(direction) {}

FlexLayout::~FlexLayout() {
    for (auto& item : m_items) {
        delete item.item;
    }
    m_items.clear();
}

void FlexLayout::addItem(LayoutItem* item) {
    if (item) {
        m_items.emplace_back(item);
        invalidate();
    }
}

int FlexLayout::count() const {
    return static_cast<int>(m_items.size());
}

LayoutItem* FlexLayout::itemAt(int index) const {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        return m_items[index].item;
    }
    return nullptr;
}

LayoutItem* FlexLayout::takeAt(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        LayoutItem* item = m_items[index].item;
        m_items.erase(m_items.begin() + index);
        invalidate();
        return item;
    }
    return nullptr;
}

void FlexLayout::setFlexDirection(FlexDirection direction) {
    if (m_direction != direction) {
        m_direction = direction;
        invalidate();
    }
}

void FlexLayout::setFlexWrap(FlexWrap wrap) {
    if (m_wrap != wrap) {
        m_wrap = wrap;
        invalidate();
    }
}

void FlexLayout::setJustifyContent(JustifyContent justify) {
    if (m_justifyContent != justify) {
        m_justifyContent = justify;
        invalidate();
    }
}

void FlexLayout::setAlignItems(AlignItems align) {
    if (m_alignItems != align) {
        m_alignItems = align;
        invalidate();
    }
}

void FlexLayout::setAlignContent(AlignContent align) {
    if (m_alignContent != align) {
        m_alignContent = align;
        invalidate();
    }
}

void FlexLayout::setGap(int mainGap, int crossGap) {
    if (crossGap == -1) crossGap = mainGap;
    
    if (m_mainGap != mainGap || m_crossGap != crossGap) {
        m_mainGap = mainGap;
        m_crossGap = crossGap;
        invalidate();
    }
}

void FlexLayout::setFlexGrow(Widget* widget, int grow) {
    if (auto item = findItemForWidget(widget)) {
        item->properties.grow = grow;
        invalidate();
    }
}

void FlexLayout::setFlexShrink(Widget* widget, int shrink) {
    if (auto item = findItemForWidget(widget)) {
        item->properties.shrink = shrink;
        invalidate();
    }
}

void FlexLayout::setFlexBasis(Widget* widget, int basis) {
    if (auto item = findItemForWidget(widget)) {
        item->properties.basis = basis;
        invalidate();
    }
}

void FlexLayout::setAlignSelf(Widget* widget, AlignItems align) {
    if (auto item = findItemForWidget(widget)) {
        item->properties.alignSelf = align;
        invalidate();
    }
}

int FlexLayout::getFlexGrow(Widget* widget) const {
    if (auto item = findItemForWidget(widget)) {
        return item->properties.grow;
    }
    return 0;
}

int FlexLayout::getFlexShrink(Widget* widget) const {
    if (auto item = findItemForWidget(widget)) {
        return item->properties.shrink;
    }
    return 1;
}

int FlexLayout::getFlexBasis(Widget* widget) const {
    if (auto item = findItemForWidget(widget)) {
        return item->properties.basis;
    }
    return -1;
}

AlignItems FlexLayout::getAlignSelf(Widget* widget) const {
    if (auto item = findItemForWidget(widget)) {
        return item->properties.alignSelf;
    }
    return AlignItems::Start;
}

void FlexLayout::setFlexProperties(Widget* widget, int grow, int shrink, int basis) {
    if (auto item = findItemForWidget(widget)) {
        item->properties.grow = grow;
        item->properties.shrink = shrink;
        item->properties.basis = basis;
        invalidate();
    }
}

void FlexLayout::makeItemFlexible(Widget* widget) {
    setFlexProperties(widget, 1, 1, -1);
}

void FlexLayout::makeItemFixed(Widget* widget) {
    setFlexProperties(widget, 0, 0, -1);
}

void FlexLayout::setGeometry(const Rect& rect) {
    if (m_items.empty()) return;
    
    // 创建 flex 行
    std::vector<FlexLine> lines = createFlexLines(rect);
    
    // 解析弹性长度
    for (auto& line : lines) {
        int availableMainSize = isMainAxisHorizontal() ? rect.width : rect.height;
        availableMainSize -= (static_cast<int>(line.items.size()) - 1) * m_mainGap;
        resolveFlexibleLengths(line, availableMainSize);
    }
    
    // 对齐内容
    alignContent(lines, rect);
}

Size FlexLayout::calculateSizeHint() const {
    return calculateMinimumSize(); // 简化实现
}

Size FlexLayout::calculateMinimumSize() const {
    if (m_items.empty()) {
        return Size(0, 0);
    }
    
    int mainSize = 0;
    int crossSize = 0;
    
    for (const auto& item : m_items) {
        if (item.item->isEmpty()) continue;
        
        Size itemSize = item.item->sizeHint();
        int itemMainSize = getMainAxisSize(itemSize);
        int itemCrossSize = getCrossAxisSize(itemSize);
        
        if (m_wrap == FlexWrap::NoWrap) {
            mainSize += itemMainSize;
            crossSize = std::max(crossSize, itemCrossSize);
        } else {
            // 换行情况下的计算更复杂，这里简化
            mainSize = std::max(mainSize, itemMainSize);
            crossSize += itemCrossSize;
        }
    }
    
    // 添加间距
    if (m_wrap == FlexWrap::NoWrap && !m_items.empty()) {
        mainSize += (static_cast<int>(m_items.size()) - 1) * m_mainGap;
    }
    
    return makeSize(mainSize, crossSize);
}

Size FlexLayout::calculateMaximumSize() const {
    return Size(16777215, 16777215);
}

bool FlexLayout::isMainAxisHorizontal() const {
    return m_direction == FlexDirection::Row || m_direction == FlexDirection::RowReverse;
}

int FlexLayout::getMainAxisSize(const Size& size) const {
    return isMainAxisHorizontal() ? size.width : size.height;
}

int FlexLayout::getCrossAxisSize(const Size& size) const {
    return isMainAxisHorizontal() ? size.height : size.width;
}

int FlexLayout::getMainAxisPosition(const Point& pos) const {
    return isMainAxisHorizontal() ? pos.x : pos.y;
}

int FlexLayout::getCrossAxisPosition(const Point& pos) const {
    return isMainAxisHorizontal() ? pos.y : pos.x;
}

Size FlexLayout::makeSize(int mainSize, int crossSize) const {
    return isMainAxisHorizontal() ? Size(mainSize, crossSize) : Size(crossSize, mainSize);
}

Point FlexLayout::makePoint(int mainPos, int crossPos) const {
    return isMainAxisHorizontal() ? Point(mainPos, crossPos) : Point(crossPos, mainPos);
}

FlexLayoutItem* FlexLayout::findItemForWidget(Widget* widget) {
    for (auto& item : m_items) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(item.item)) {
            if (widgetItem->widget() == widget) {
                return &item;
            }
        }
    }
    return nullptr;
}

const FlexLayoutItem* FlexLayout::findItemForWidget(Widget* widget) const {
    for (const auto& item : m_items) {
        if (auto widgetItem = dynamic_cast<WidgetItem*>(item.item)) {
            if (widgetItem->widget() == widget) {
                return &item;
            }
        }
    }
    return nullptr;
}

std::vector<FlexLayout::FlexLine> FlexLayout::createFlexLines(const Rect& containerRect) const {
    std::vector<FlexLine> lines;
    
    if (m_items.empty()) return lines;
    
    FlexLine currentLine;
    int currentLineMainSize = 0;
    const int containerMainSize = getMainAxisSize(Size(containerRect.width, containerRect.height));
    
    for (const auto& item : m_items) {
        if (item.item->isEmpty()) continue;
        
        int itemMainSize = getItemMainSize(item);
        int itemCrossSize = getItemCrossSize(item);
        
        // 检查是否需要换行
        bool needWrap = false;
        if (m_wrap != FlexWrap::NoWrap && !currentLine.items.empty()) {
            int totalMainSize = currentLineMainSize + itemMainSize;
            if (!currentLine.items.empty()) {
                totalMainSize += m_mainGap;
            }
            needWrap = totalMainSize > containerMainSize;
        }
        
        if (needWrap) {
            // 完成当前行
            if (!currentLine.items.empty()) {
                lines.push_back(currentLine);
            }
            
            // 开始新行
            currentLine = FlexLine();
            currentLineMainSize = 0;
        }
        
        // 添加项目到当前行
        currentLine.items.push_back(const_cast<FlexLayoutItem*>(&item));
        currentLine.mainSize = currentLineMainSize + itemMainSize;
        currentLine.crossSize = std::max(currentLine.crossSize, itemCrossSize);
        currentLine.totalGrow += item.properties.grow;
        currentLine.totalShrink += item.properties.shrink;
        currentLine.totalBasis += getItemFlexBasis(item);
        
        currentLineMainSize += itemMainSize;
        if (!currentLine.items.empty() && currentLine.items.size() > 1) {
            currentLineMainSize += m_mainGap;
        }
    }
    
    // 添加最后一行
    if (!currentLine.items.empty()) {
        lines.push_back(currentLine);
    }
    
    return lines;
}

void FlexLayout::resolveFlexibleLengths(FlexLine& line, int availableMainSize) const {
    if (line.items.empty()) return;
    
    // 计算初始主轴尺寸
    int usedMainSize = 0;
    for (auto item : line.items) {
        int itemMainSize = getItemMainSize(*item);
        usedMainSize += itemMainSize;
    }
    
    int freeSpace = availableMainSize - usedMainSize;
    
    if (freeSpace > 0 && line.totalGrow > 0) {
        // 扩展项目
        for (auto item : line.items) {
            if (item->properties.grow > 0) {
                int extraSpace = (freeSpace * item->properties.grow) / line.totalGrow;
                // 这里应该更新项目的实际尺寸，简化实现
            }
        }
    } else if (freeSpace < 0 && line.totalShrink > 0) {
        // 收缩项目
        int deficitSpace = -freeSpace;
        for (auto item : line.items) {
            if (item->properties.shrink > 0) {
                int reduceSpace = (deficitSpace * item->properties.shrink) / line.totalShrink;
                // 这里应该更新项目的实际尺寸，简化实现
            }
        }
    }
}

void FlexLayout::alignItemsInLine(const FlexLine& line, const Rect& lineRect) const {
    for (auto item : line.items) {
        AlignItems align = (item->properties.alignSelf != AlignItems::Start) ? 
                          item->properties.alignSelf : m_alignItems;
        
        Size itemSize = item->item->sizeHint();
        int itemCrossSize = getCrossAxisSize(itemSize);
        int lineCrossSize = getCrossAxisSize(Size(lineRect.width, lineRect.height));
        
        int crossPos = getCrossAxisPosition(lineRect.topLeft());
        
        switch (align) {
        case AlignItems::Start:
            // 已经是起始位置
            break;
        case AlignItems::End:
            crossPos += lineCrossSize - itemCrossSize;
            break;
        case AlignItems::Center:
            crossPos += (lineCrossSize - itemCrossSize) / 2;
            break;
        case AlignItems::Stretch:
            itemCrossSize = lineCrossSize;
            break;
        case AlignItems::Baseline:
            // 基线对齐的实现较复杂，这里简化为起始对齐
            break;
        }
        
        // 设置项目几何
        Point itemPos = makePoint(getMainAxisPosition(lineRect.topLeft()), crossPos);
        Size finalSize = makeSize(getMainAxisSize(itemSize), itemCrossSize);
        item->item->setGeometry(Rect(itemPos, finalSize));
    }
}

void FlexLayout::alignContent(const std::vector<FlexLine>& lines, const Rect& containerRect) const {
    if (lines.empty()) return;
    
    int containerCrossSize = getCrossAxisSize(Size(containerRect.width, containerRect.height));
    int totalLinesCrossSize = 0;
    
    for (const auto& line : lines) {
        totalLinesCrossSize += line.crossSize;
    }
    
    int freeCrossSpace = containerCrossSize - totalLinesCrossSize;
    int crossPos = getCrossAxisPosition(containerRect.topLeft());
    
    // 根据 alignContent 分配行的位置
    switch (m_alignContent) {
    case AlignContent::Start:
        // 已经是起始位置
        break;
    case AlignContent::End:
        crossPos += freeCrossSpace;
        break;
    case AlignContent::Center:
        crossPos += freeCrossSpace / 2;
        break;
    case AlignContent::SpaceBetween:
        if (lines.size() > 1) {
            freeCrossSpace = freeCrossSpace / static_cast<int>(lines.size() - 1);
        }
        break;
    case AlignContent::SpaceAround:
        freeCrossSpace = freeCrossSpace / static_cast<int>(lines.size());
        crossPos += freeCrossSpace / 2;
        break;
    case AlignContent::Stretch:
        // 拉伸每一行
        if (!lines.empty()) {
            int extraPerLine = freeCrossSpace / static_cast<int>(lines.size());
            for (auto& line : const_cast<std::vector<FlexLine>&>(lines)) {
                line.crossSize += extraPerLine;
            }
        }
        break;
    }
    
    // 布局每一行
    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        
        int mainPos = getMainAxisPosition(containerRect.topLeft());
        Rect lineRect = Rect(makePoint(mainPos, crossPos), 
                           makeSize(getMainAxisSize(Size(containerRect.width, containerRect.height)), 
                                   line.crossSize));
        
        justifyContent(line, lineRect);
        alignItemsInLine(line, lineRect);
        
        crossPos += line.crossSize;
        if (m_alignContent == AlignContent::SpaceBetween && i < lines.size() - 1) {
            crossPos += freeCrossSpace;
        } else if (m_alignContent == AlignContent::SpaceAround) {
            crossPos += freeCrossSpace;
        }
    }
}

void FlexLayout::justifyContent(const FlexLine& line, const Rect& lineRect) const {
    if (line.items.empty()) return;
    
    int lineMainSize = getMainAxisSize(Size(lineRect.width, lineRect.height));
    int totalItemsMainSize = 0;
    
    for (auto item : line.items) {
        totalItemsMainSize += getItemMainSize(*item);
    }
    
    int freeMainSpace = lineMainSize - totalItemsMainSize - (static_cast<int>(line.items.size()) - 1) * m_mainGap;
    int mainPos = getMainAxisPosition(lineRect.topLeft());
    
    // 根据 justifyContent 分配项目位置
    int spacing = m_mainGap;
    
    switch (m_justifyContent) {
    case JustifyContent::Start:
        // 已经是起始位置
        break;
    case JustifyContent::End:
        mainPos += freeMainSpace;
        break;
    case JustifyContent::Center:
        mainPos += freeMainSpace / 2;
        break;
    case JustifyContent::SpaceBetween:
        if (line.items.size() > 1) {
            spacing += freeMainSpace / static_cast<int>(line.items.size() - 1);
        }
        break;
    case JustifyContent::SpaceAround:
        spacing += freeMainSpace / static_cast<int>(line.items.size());
        mainPos += spacing / 2;
        break;
    case JustifyContent::SpaceEvenly:
        spacing = freeMainSpace / static_cast<int>(line.items.size() + 1);
        mainPos += spacing;
        break;
    }
    
    // 设置每个项目的主轴位置
    bool isReverse = (m_direction == FlexDirection::RowReverse || 
                     m_direction == FlexDirection::ColumnReverse);
    
    for (size_t i = 0; i < line.items.size(); ++i) {
        size_t index = isReverse ? (line.items.size() - 1 - i) : i;
        auto item = line.items[index];
        
        int itemMainSize = getItemMainSize(*item);
        
        // 这里应该设置项目的主轴位置，与 alignItemsInLine 配合
        // 简化实现，实际应该更新项目的最终几何位置
        
        mainPos += itemMainSize + spacing;
    }
}

int FlexLayout::getItemMainSize(const FlexLayoutItem& item, bool minimum) const {
    Size size = minimum ? item.item->minimumSize() : item.item->sizeHint();
    return getMainAxisSize(size);
}

int FlexLayout::getItemCrossSize(const FlexLayoutItem& item, bool minimum) const {
    Size size = minimum ? item.item->minimumSize() : item.item->sizeHint();
    return getCrossAxisSize(size);
}

int FlexLayout::getItemFlexBasis(const FlexLayoutItem& item) const {
    if (item.properties.basis >= 0) {
        return item.properties.basis;
    }
    return getItemMainSize(item);
}

} // namespace QGL