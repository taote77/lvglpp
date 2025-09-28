#ifndef QGL_LAYOUTS_GRIDLAYOUT_H
#define QGL_LAYOUTS_GRIDLAYOUT_H

#include "Layout.h"
#include <vector>
#include <map>

namespace QGL {

// 网格布局项
struct GridLayoutItem {
    LayoutItem* item = nullptr;
    int row = 0;
    int column = 0;
    int rowSpan = 1;
    int columnSpan = 1;
    Alignment alignment = Alignment::Left;
    
    GridLayoutItem() = default;
    GridLayoutItem(LayoutItem* it, int r, int c, int rs = 1, int cs = 1, Alignment align = Alignment::Left)
        : item(it), row(r), column(c), rowSpan(rs), columnSpan(cs), alignment(align) {}
};

// 网格布局
class GridLayout : public Layout {
    QGL_OBJECT(GridLayout)
    
public:
    explicit GridLayout(Widget* parent = nullptr);
    ~GridLayout() override;
    
    // Layout 接口实现
    void addItem(LayoutItem* item) override;
    int count() const override;
    LayoutItem* itemAt(int index) const override;
    LayoutItem* takeAt(int index) override;
    
    // GridLayout 特定方法
    void addWidget(Widget* widget, int row, int column, 
                   int rowSpan = 1, int columnSpan = 1, 
                   Alignment alignment = Alignment::Left);
    void addItem(LayoutItem* item, int row, int column,
                 int rowSpan = 1, int columnSpan = 1,
                 Alignment alignment = Alignment::Left);
    
    // 布局信息查询
    int rowCount() const;
    int columnCount() const;
    LayoutItem* itemAtPosition(int row, int column) const;
    
    // 行列属性设置
    void setRowStretch(int row, int stretch);
    void setColumnStretch(int column, int stretch);
    int getRowStretch(int row) const;
    int getColumnStretch(int column) const;
    
    void setRowMinimumHeight(int row, int minSize);
    void setColumnMinimumWidth(int column, int minSize);
    int getRowMinimumHeight(int row) const;
    int getColumnMinimumWidth(int column) const;
    
    void setRowFixedHeight(int row, int height);
    void setColumnFixedWidth(int column, int width);
    
    // 间距设置
    void setHorizontalSpacing(int spacing);
    void setVerticalSpacing(int spacing);
    int getHorizontalSpacing() const { return m_horizontalSpacing; }
    int getVerticalSpacing() const { return m_verticalSpacing; }
    
protected:
    // Layout 纯虚函数实现
    void setGeometry(const Rect& rect) override;
    Size calculateSizeHint() const override;
    Size calculateMinimumSize() const override;
    Size calculateMaximumSize() const override;
    
private:
    std::vector<GridLayoutItem> m_items;
    std::map<int, int> m_rowStretches;
    std::map<int, int> m_columnStretches;
    std::map<int, int> m_rowMinSizes;
    std::map<int, int> m_columnMinSizes;
    std::map<int, int> m_rowFixedSizes;
    std::map<int, int> m_columnFixedSizes;
    
    int m_horizontalSpacing = 6;
    int m_verticalSpacing = 6;
    
    // 工具方法
    void updateGridDimensions();
    std::vector<int> calculateRowSizes(int availableHeight) const;
    std::vector<int> calculateColumnSizes(int availableWidth) const;
    Size calculateGridSize(bool minimum) const;
    
    int getEffectiveRowStretch(int row) const;
    int getEffectiveColumnStretch(int column) const;
    int getEffectiveRowMinSize(int row) const;
    int getEffectiveColumnMinSize(int column) const;
    
    bool hasFixedSize(int index, const std::map<int, int>& fixedSizes) const;
    int getFixedSize(int index, const std::map<int, int>& fixedSizes) const;
    
    void expandItemRect(Rect& rect, const GridLayoutItem& item,
                       const std::vector<int>& rowSizes,
                       const std::vector<int>& columnSizes,
                       int startRow, int startColumn) const;
    
    int m_cachedRowCount = 0;
    int m_cachedColumnCount = 0;
};

} // namespace QGL

#endif // QGL_LAYOUTS_GRIDLAYOUT_H