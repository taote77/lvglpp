#include "GridLayout.h"
#include "../widgets/Widget.h"
#include <algorithm>

namespace QGL {

GridLayout::GridLayout(Widget* parent) : Layout(parent) {}

GridLayout::~GridLayout() {
    for (auto& item : m_items) {
        delete item.item;
    }
    m_items.clear();
}

void GridLayout::addItem(LayoutItem* item) {
    if (item) {
        // 找到下一个可用位置
        int row = 0, column = 0;
        while (itemAtPosition(row, column) != nullptr) {
            column++;
            if (column >= columnCount()) {
                column = 0;
                row++;
            }
        }
        addItem(item, row, column);
    }
}

int GridLayout::count() const {
    return static_cast<int>(m_items.size());
}

LayoutItem* GridLayout::itemAt(int index) const {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        return m_items[index].item;
    }
    return nullptr;
}

LayoutItem* GridLayout::takeAt(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        LayoutItem* item = m_items[index].item;
        m_items.erase(m_items.begin() + index);
        updateGridDimensions();
        invalidate();
        return item;
    }
    return nullptr;
}

void GridLayout::addWidget(Widget* widget, int row, int column,
                          int rowSpan, int columnSpan, Alignment alignment) {
    if (widget) {
        addItem(new WidgetItem(widget), row, column, rowSpan, columnSpan, alignment);
    }
}

void GridLayout::addItem(LayoutItem* item, int row, int column,
                        int rowSpan, int columnSpan, Alignment alignment) {
    if (item && row >= 0 && column >= 0 && rowSpan > 0 && columnSpan > 0) {
        m_items.emplace_back(item, row, column, rowSpan, columnSpan, alignment);
        updateGridDimensions();
        invalidate();
    }
}

int GridLayout::rowCount() const {
    return m_cachedRowCount;
}

int GridLayout::columnCount() const {
    return m_cachedColumnCount;
}

LayoutItem* GridLayout::itemAtPosition(int row, int column) const {
    for (const auto& item : m_items) {
        if (item.row <= row && row < item.row + item.rowSpan &&
            item.column <= column && column < item.column + item.columnSpan) {
            return item.item;
        }
    }
    return nullptr;
}

void GridLayout::setRowStretch(int row, int stretch) {
    if (row >= 0) {
        m_rowStretches[row] = stretch;
        invalidate();
    }
}

void GridLayout::setColumnStretch(int column, int stretch) {
    if (column >= 0) {
        m_columnStretches[column] = stretch;
        invalidate();
    }
}

int GridLayout::getRowStretch(int row) const {
    return getEffectiveRowStretch(row);
}

int GridLayout::getColumnStretch(int column) const {
    return getEffectiveColumnStretch(column);
}

void GridLayout::setRowMinimumHeight(int row, int minSize) {
    if (row >= 0) {
        m_rowMinSizes[row] = minSize;
        invalidate();
    }
}

void GridLayout::setColumnMinimumWidth(int column, int minSize) {
    if (column >= 0) {
        m_columnMinSizes[column] = minSize;
        invalidate();
    }
}

int GridLayout::getRowMinimumHeight(int row) const {
    return getEffectiveRowMinSize(row);
}

int GridLayout::getColumnMinimumWidth(int column) const {
    return getEffectiveColumnMinSize(column);
}

void GridLayout::setRowFixedHeight(int row, int height) {
    if (row >= 0) {
        m_rowFixedSizes[row] = height;
        invalidate();
    }
}

void GridLayout::setColumnFixedWidth(int column, int width) {
    if (column >= 0) {
        m_columnFixedSizes[column] = width;
        invalidate();
    }
}

void GridLayout::setHorizontalSpacing(int spacing) {
    if (m_horizontalSpacing != spacing) {
        m_horizontalSpacing = spacing;
        invalidate();
    }
}

void GridLayout::setVerticalSpacing(int spacing) {
    if (m_verticalSpacing != spacing) {
        m_verticalSpacing = spacing;
        invalidate();
    }
}

void GridLayout::setGeometry(const Rect& rect) {
    if (m_items.empty()) return;
    
    updateGridDimensions();
    
    const int rows = rowCount();
    const int columns = columnCount();
    
    if (rows == 0 || columns == 0) return;
    
    // 计算可用空间
    const int totalHSpacing = (columns - 1) * m_horizontalSpacing;
    const int totalVSpacing = (rows - 1) * m_verticalSpacing;
    const int availableWidth = rect.width - totalHSpacing;
    const int availableHeight = rect.height - totalVSpacing;
    
    // 计算行列尺寸
    std::vector<int> rowSizes = calculateRowSizes(availableHeight);
    std::vector<int> columnSizes = calculateColumnSizes(availableWidth);
    
    // 布局每个项目
    for (const auto& gridItem : m_items) {
        if (gridItem.item->isEmpty()) continue;
        
        // 计算项目区域
        int x = rect.x;
        int y = rect.y;
        int width = 0;
        int height = 0;
        
        // 计算水平位置和宽度
        for (int c = 0; c < gridItem.column && c < columns; ++c) {
            x += columnSizes[c] + m_horizontalSpacing;
        }
        for (int c = gridItem.column; c < gridItem.column + gridItem.columnSpan && c < columns; ++c) {
            if (c > gridItem.column) width += m_horizontalSpacing;
            width += columnSizes[c];
        }
        
        // 计算垂直位置和高度
        for (int r = 0; r < gridItem.row && r < rows; ++r) {
            y += rowSizes[r] + m_verticalSpacing;
        }
        for (int r = gridItem.row; r < gridItem.row + gridItem.rowSpan && r < rows; ++r) {
            if (r > gridItem.row) height += m_verticalSpacing;
            height += rowSizes[r];
        }
        
        Rect itemRect(x, y, width, height);
        
        // 应用对齐
        Size itemSizeHint = gridItem.item->sizeHint();
        if (itemSizeHint.width < width || itemSizeHint.height < height) {
            itemRect = alignmentRect(itemRect, itemSizeHint, gridItem.alignment);
        }
        
        gridItem.item->setGeometry(itemRect);
    }
}

Size GridLayout::calculateSizeHint() const {
    return calculateGridSize(false);
}

Size GridLayout::calculateMinimumSize() const {
    return calculateGridSize(true);
}

Size GridLayout::calculateMaximumSize() const {
    return Size(16777215, 16777215);
}

void GridLayout::updateGridDimensions() {
    m_cachedRowCount = 0;
    m_cachedColumnCount = 0;
    
    for (const auto& item : m_items) {
        m_cachedRowCount = std::max(m_cachedRowCount, item.row + item.rowSpan);
        m_cachedColumnCount = std::max(m_cachedColumnCount, item.column + item.columnSpan);
    }
}

std::vector<int> GridLayout::calculateRowSizes(int availableHeight) const {
    const int rows = rowCount();
    std::vector<int> sizes(rows, 0);
    
    if (rows == 0) return sizes;
    
    // 计算每行的最小尺寸
    for (const auto& item : m_items) {
        if (item.item->isEmpty()) continue;
        
        Size itemMinSize = item.item->minimumSize();
        Size itemSizeHint = item.item->sizeHint();
        int itemHeight = std::max(itemMinSize.height, itemSizeHint.height);
        
        if (item.rowSpan == 1) {
            sizes[item.row] = std::max(sizes[item.row], itemHeight);
        }
    }
    
    // 应用最小尺寸约束
    for (int r = 0; r < rows; ++r) {
        sizes[r] = std::max(sizes[r], getEffectiveRowMinSize(r));
    }
    
    // 应用固定尺寸
    for (int r = 0; r < rows; ++r) {
        if (hasFixedSize(r, m_rowFixedSizes)) {
            sizes[r] = getFixedSize(r, m_rowFixedSizes);
        }
    }
    
    // 分配剩余空间
    int usedHeight = 0;
    for (int size : sizes) {
        usedHeight += size;
    }
    
    int remainingHeight = availableHeight - usedHeight;
    if (remainingHeight > 0) {
        int totalStretch = 0;
        for (int r = 0; r < rows; ++r) {
            if (!hasFixedSize(r, m_rowFixedSizes)) {
                totalStretch += getEffectiveRowStretch(r);
            }
        }
        
        if (totalStretch > 0) {
            for (int r = 0; r < rows; ++r) {
                if (!hasFixedSize(r, m_rowFixedSizes)) {
                    int stretch = getEffectiveRowStretch(r);
                    sizes[r] += (remainingHeight * stretch) / totalStretch;
                }
            }
        }
    }
    
    return sizes;
}

std::vector<int> GridLayout::calculateColumnSizes(int availableWidth) const {
    const int columns = columnCount();
    std::vector<int> sizes(columns, 0);
    
    if (columns == 0) return sizes;
    
    // 计算每列的最小尺寸
    for (const auto& item : m_items) {
        if (item.item->isEmpty()) continue;
        
        Size itemMinSize = item.item->minimumSize();
        Size itemSizeHint = item.item->sizeHint();
        int itemWidth = std::max(itemMinSize.width, itemSizeHint.width);
        
        if (item.columnSpan == 1) {
            sizes[item.column] = std::max(sizes[item.column], itemWidth);
        }
    }
    
    // 应用最小尺寸约束
    for (int c = 0; c < columns; ++c) {
        sizes[c] = std::max(sizes[c], getEffectiveColumnMinSize(c));
    }
    
    // 应用固定尺寸
    for (int c = 0; c < columns; ++c) {
        if (hasFixedSize(c, m_columnFixedSizes)) {
            sizes[c] = getFixedSize(c, m_columnFixedSizes);
        }
    }
    
    // 分配剩余空间
    int usedWidth = 0;
    for (int size : sizes) {
        usedWidth += size;
    }
    
    int remainingWidth = availableWidth - usedWidth;
    if (remainingWidth > 0) {
        int totalStretch = 0;
        for (int c = 0; c < columns; ++c) {
            if (!hasFixedSize(c, m_columnFixedSizes)) {
                totalStretch += getEffectiveColumnStretch(c);
            }
        }
        
        if (totalStretch > 0) {
            for (int c = 0; c < columns; ++c) {
                if (!hasFixedSize(c, m_columnFixedSizes)) {
                    int stretch = getEffectiveColumnStretch(c);
                    sizes[c] += (remainingWidth * stretch) / totalStretch;
                }
            }
        }
    }
    
    return sizes;
}

Size GridLayout::calculateGridSize(bool minimum) const {
    const_cast<GridLayout*>(this)->updateGridDimensions();
    
    const int rows = rowCount();
    const int columns = columnCount();
    
    if (rows == 0 || columns == 0) {
        return Size(0, 0);
    }
    
    std::vector<int> rowSizes(rows, 0);
    std::vector<int> columnSizes(columns, 0);
    
    // 计算行列尺寸
    for (const auto& item : m_items) {
        if (item.item->isEmpty()) continue;
        
        Size itemSize = minimum ? item.item->minimumSize() : item.item->sizeHint();
        
        if (item.rowSpan == 1) {
            rowSizes[item.row] = std::max(rowSizes[item.row], itemSize.height);
        }
        if (item.columnSpan == 1) {
            columnSizes[item.column] = std::max(columnSizes[item.column], itemSize.width);
        }
    }
    
    // 应用约束
    for (int r = 0; r < rows; ++r) {
        rowSizes[r] = std::max(rowSizes[r], getEffectiveRowMinSize(r));
        if (hasFixedSize(r, m_rowFixedSizes)) {
            rowSizes[r] = getFixedSize(r, m_rowFixedSizes);
        }
    }
    
    for (int c = 0; c < columns; ++c) {
        columnSizes[c] = std::max(columnSizes[c], getEffectiveColumnMinSize(c));
        if (hasFixedSize(c, m_columnFixedSizes)) {
            columnSizes[c] = getFixedSize(c, m_columnFixedSizes);
        }
    }
    
    // 计算总尺寸
    int totalWidth = 0;
    int totalHeight = 0;
    
    for (int size : columnSizes) {
        totalWidth += size;
    }
    for (int size : rowSizes) {
        totalHeight += size;
    }
    
    totalWidth += (columns - 1) * m_horizontalSpacing;
    totalHeight += (rows - 1) * m_verticalSpacing;
    
    return Size(totalWidth, totalHeight);
}

int GridLayout::getEffectiveRowStretch(int row) const {
    auto it = m_rowStretches.find(row);
    return (it != m_rowStretches.end()) ? it->second : 0;
}

int GridLayout::getEffectiveColumnStretch(int column) const {
    auto it = m_columnStretches.find(column);
    return (it != m_columnStretches.end()) ? it->second : 0;
}

int GridLayout::getEffectiveRowMinSize(int row) const {
    auto it = m_rowMinSizes.find(row);
    return (it != m_rowMinSizes.end()) ? it->second : 0;
}

int GridLayout::getEffectiveColumnMinSize(int column) const {
    auto it = m_columnMinSizes.find(column);
    return (it != m_columnMinSizes.end()) ? it->second : 0;
}

bool GridLayout::hasFixedSize(int index, const std::map<int, int>& fixedSizes) const {
    return fixedSizes.find(index) != fixedSizes.end();
}

int GridLayout::getFixedSize(int index, const std::map<int, int>& fixedSizes) const {
    auto it = fixedSizes.find(index);
    return (it != fixedSizes.end()) ? it->second : 0;
}

} // namespace QGL