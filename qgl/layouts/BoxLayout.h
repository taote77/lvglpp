#ifndef QGL_LAYOUTS_BOXLAYOUT_H
#define QGL_LAYOUTS_BOXLAYOUT_H

#include "Layout.h"
#include <vector>

namespace QGL {

// 盒子布局方向
enum class LayoutDirection {
    LeftToRight,
    RightToLeft,
    TopToBottom,
    BottomToTop
};

// 盒子布局基类
class BoxLayout : public Layout {
    QGL_OBJECT(BoxLayout)
    
public:
    explicit BoxLayout(LayoutDirection direction, Widget* parent = nullptr);
    ~BoxLayout() override;
    
    // Layout 接口实现
    void addItem(LayoutItem* item) override;
    int count() const override;
    LayoutItem* itemAt(int index) const override;
    LayoutItem* takeAt(int index) override;
    
    // BoxLayout 特定方法
    void insertItem(int index, LayoutItem* item);
    void insertWidget(int index, Widget* widget);
    void insertSpacing(int index, int size);
    void insertStretch(int index, int stretch = 0);
    
    // 布局方向
    void setDirection(LayoutDirection direction);
    LayoutDirection getDirection() const { return m_direction; }
    
    // 拉伸因子
    void setStretch(int index, int stretch);
    int getStretch(int index) const;
    
    // 对齐方式
    void setAlignment(Widget* widget, Alignment alignment);
    Alignment getAlignment(Widget* widget) const;
    
protected:
    // Layout 纯虚函数实现
    void setGeometry(const Rect& rect) override;
    Size calculateSizeHint() const override;
    Size calculateMinimumSize() const override;
    Size calculateMaximumSize() const override;
    
private:
    struct BoxLayoutItem {
        LayoutItem* item;
        int stretch;
        Alignment alignment;
        
        BoxLayoutItem(LayoutItem* it, int str = 0, Alignment align = Alignment::Left)
            : item(it), stretch(str), alignment(align) {}
    };
    
    std::vector<BoxLayoutItem> m_items;
    LayoutDirection m_direction;
    
    // 工具方法
    bool isHorizontal() const;
    void distributeSpace(const Rect& rect);
    Size calculateSize(bool minimum) const;
    int totalStretch() const;
    std::vector<int> calculateSizes(int available) const;
};

// 水平布局
class HBoxLayout : public BoxLayout {
    QGL_OBJECT(HBoxLayout)
    
public:
    explicit HBoxLayout(Widget* parent = nullptr)
        : BoxLayout(LayoutDirection::LeftToRight, parent) {}
};

// 垂直布局
class VBoxLayout : public BoxLayout {
    QGL_OBJECT(VBoxLayout)
    
public:
    explicit VBoxLayout(Widget* parent = nullptr)
        : BoxLayout(LayoutDirection::TopToBottom, parent) {}
};

} // namespace QGL

#endif // QGL_LAYOUTS_BOXLAYOUT_H