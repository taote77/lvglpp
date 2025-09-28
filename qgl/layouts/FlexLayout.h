#ifndef QGL_LAYOUTS_FLEXLAYOUT_H
#define QGL_LAYOUTS_FLEXLAYOUT_H

#include "Layout.h"
#include <vector>
#include <map>

namespace QGL {

// Flex 布局方向
enum class FlexDirection {
    Row,           // 水平方向，从左到右
    RowReverse,    // 水平方向，从右到左  
    Column,        // 垂直方向，从上到下
    ColumnReverse  // 垂直方向，从下到上
};

// Flex 换行方式
enum class FlexWrap {
    NoWrap,      // 不换行
    Wrap,        // 换行
    WrapReverse  // 反向换行
};

// Flex 主轴对齐方式
enum class JustifyContent {
    Start,        // 起始对齐
    End,          // 结束对齐
    Center,       // 居中对齐
    SpaceBetween, // 两端对齐，中间均匀分布
    SpaceAround,  // 环绕对齐，每个项目两侧间距相等
    SpaceEvenly   // 均匀分布，所有间距相等
};

// Flex 交叉轴对齐方式
enum class AlignItems {
    Start,    // 交叉轴起始对齐
    End,      // 交叉轴结束对齐
    Center,   // 交叉轴居中对齐
    Stretch,  // 拉伸填充
    Baseline  // 基线对齐
};

// Flex 多行对齐方式
enum class AlignContent {
    Start,        // 起始对齐
    End,          // 结束对齐
    Center,       // 居中对齐
    Stretch,      // 拉伸对齐
    SpaceBetween, // 两端对齐
    SpaceAround   // 环绕对齐
};

// Flex 项目属性
struct FlexItemProperties {
    int grow = 0;           // 扩展比例
    int shrink = 1;         // 收缩比例
    int basis = -1;         // 基准尺寸 (-1 表示 auto)
    AlignItems alignSelf = AlignItems::Start;  // 自身对齐方式
};

// Flex 布局项
struct FlexLayoutItem {
    LayoutItem* item = nullptr;
    FlexItemProperties properties;
    
    FlexLayoutItem(LayoutItem* it) : item(it) {}
    FlexLayoutItem(LayoutItem* it, const FlexItemProperties& props)
        : item(it), properties(props) {}
};

// Flex 布局
class FlexLayout : public Layout {
    QGL_OBJECT(FlexLayout)
    
public:
    explicit FlexLayout(FlexDirection direction = FlexDirection::Row, Widget* parent = nullptr);
    ~FlexLayout() override;
    
    // Layout 接口实现
    void addItem(LayoutItem* item) override;
    int count() const override;
    LayoutItem* itemAt(int index) const override;
    LayoutItem* takeAt(int index) override;
    
    // Flex 容器属性设置
    void setFlexDirection(FlexDirection direction);
    FlexDirection getFlexDirection() const { return m_direction; }
    
    void setFlexWrap(FlexWrap wrap);
    FlexWrap getFlexWrap() const { return m_wrap; }
    
    void setJustifyContent(JustifyContent justify);
    JustifyContent getJustifyContent() const { return m_justifyContent; }
    
    void setAlignItems(AlignItems align);
    AlignItems getAlignItems() const { return m_alignItems; }
    
    void setAlignContent(AlignContent align);
    AlignContent getAlignContent() const { return m_alignContent; }
    
    void setGap(int mainGap, int crossGap = -1);
    int getMainGap() const { return m_mainGap; }
    int getCrossGap() const { return m_crossGap; }
    
    // Flex 子项属性设置
    void setFlexGrow(Widget* widget, int grow);
    void setFlexShrink(Widget* widget, int shrink);
    void setFlexBasis(Widget* widget, int basis);
    void setAlignSelf(Widget* widget, AlignItems align);
    
    int getFlexGrow(Widget* widget) const;
    int getFlexShrink(Widget* widget) const;
    int getFlexBasis(Widget* widget) const;
    AlignItems getAlignSelf(Widget* widget) const;
    
    // 便捷方法
    void setFlexProperties(Widget* widget, int grow, int shrink, int basis);
    void makeItemFlexible(Widget* widget);  // grow=1, shrink=1, basis=auto
    void makeItemFixed(Widget* widget);     // grow=0, shrink=0, basis=content
    
protected:
    // Layout 纯虚函数实现
    void setGeometry(const Rect& rect) override;
    Size calculateSizeHint() const override;
    Size calculateMinimumSize() const override;
    Size calculateMaximumSize() const override;
    
private:
    std::vector<FlexLayoutItem> m_items;
    
    // Flex 容器属性
    FlexDirection m_direction = FlexDirection::Row;
    FlexWrap m_wrap = FlexWrap::NoWrap;
    JustifyContent m_justifyContent = JustifyContent::Start;
    AlignItems m_alignItems = AlignItems::Stretch;
    AlignContent m_alignContent = AlignContent::Stretch;
    int m_mainGap = 0;
    int m_crossGap = 0;
    
    // 工具方法
    bool isMainAxisHorizontal() const;
    int getMainAxisSize(const Size& size) const;
    int getCrossAxisSize(const Size& size) const;
    int getMainAxisPosition(const Point& pos) const;
    int getCrossAxisPosition(const Point& pos) const;
    
    Size makeSize(int mainSize, int crossSize) const;
    Point makePoint(int mainPos, int crossPos) const;
    
    FlexLayoutItem* findItemForWidget(Widget* widget);
    const FlexLayoutItem* findItemForWidget(Widget* widget) const;
    
    // Flex 布局算法
    struct FlexLine {
        std::vector<FlexLayoutItem*> items;
        int mainSize = 0;
        int crossSize = 0;
        int totalGrow = 0;
        int totalShrink = 0;
        int totalBasis = 0;
    };
    
    std::vector<FlexLine> createFlexLines(const Rect& containerRect) const;
    void resolveFlexibleLengths(FlexLine& line, int availableMainSize) const;
    void alignItemsInLine(const FlexLine& line, const Rect& lineRect) const;
    void alignContent(const std::vector<FlexLine>& lines, const Rect& containerRect) const;
    void justifyContent(const FlexLine& line, const Rect& lineRect) const;
    
    int getItemMainSize(const FlexLayoutItem& item, bool minimum = false) const;
    int getItemCrossSize(const FlexLayoutItem& item, bool minimum = false) const;
    int getItemFlexBasis(const FlexLayoutItem& item) const;
};

} // namespace QGL

#endif // QGL_LAYOUTS_FLEXLAYOUT_H