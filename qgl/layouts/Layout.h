#ifndef QGL_LAYOUTS_LAYOUT_H
#define QGL_LAYOUTS_LAYOUT_H

#include "../core/Object.h"
#include "../core/Types.h"
#include <vector>

namespace QGL {

// 前向声明
class Widget;

// 布局项接口
class LayoutItem {
public:
    virtual ~LayoutItem() = default;
    
    virtual Size sizeHint() const = 0;
    virtual Size minimumSize() const = 0;
    virtual Size maximumSize() const = 0;
    virtual void setGeometry(const Rect& rect) = 0;
    virtual Rect geometry() const = 0;
    virtual bool isEmpty() const = 0;
    
    // 扩展策略
    virtual int horizontalStretch() const { return 0; }
    virtual int verticalStretch() const { return 0; }
    virtual void setHorizontalStretch(int stretch) {}
    virtual void setVerticalStretch(int stretch) {}
};

// 控件布局项
class WidgetItem : public LayoutItem {
public:
    explicit WidgetItem(Widget* widget);
    
    Size sizeHint() const override;
    Size minimumSize() const override;
    Size maximumSize() const override;
    void setGeometry(const Rect& rect) override;
    Rect geometry() const override;
    bool isEmpty() const override;
    
    Widget* widget() const { return m_widget; }
    
private:
    Widget* m_widget;
};

// 间隔布局项
class SpacerItem : public LayoutItem {
public:
    SpacerItem(int width, int height, 
               int hStretch = 0, int vStretch = 0);
    
    Size sizeHint() const override;
    Size minimumSize() const override;
    Size maximumSize() const override;
    void setGeometry(const Rect& rect) override;
    Rect geometry() const override;
    bool isEmpty() const override;
    
    int horizontalStretch() const override { return m_hStretch; }
    int verticalStretch() const override { return m_vStretch; }
    
private:
    Size m_size;
    Rect m_geometry;
    int m_hStretch;
    int m_vStretch;
};

// 布局基类
class Layout : public Object {
    QGL_OBJECT(Layout)
    
public:
    explicit Layout(Widget* parent = nullptr);
    ~Layout() override;
    
    // 布局项管理
    virtual void addItem(LayoutItem* item) = 0;
    virtual int count() const = 0;
    virtual LayoutItem* itemAt(int index) const = 0;
    virtual LayoutItem* takeAt(int index) = 0;
    
    // 控件便捷方法
    virtual void addWidget(Widget* widget);
    virtual void removeWidget(Widget* widget);
    virtual int indexOf(Widget* widget) const;
    
    // 间隔便捷方法
    virtual void addSpacing(int size);
    virtual void addStretch(int stretch = 0);
    
    // 布局属性
    void setSpacing(int spacing);
    int getSpacing() const { return m_spacing; }
    
    void setMargins(int left, int top, int right, int bottom);
    void setMargins(int margin) { setMargins(margin, margin, margin, margin); }
    void setLeftMargin(int margin) { m_margins.x = margin; }
    void setTopMargin(int margin) { m_margins.y = margin; }
    void setRightMargin(int margin) { m_margins.width = margin; }
    void setBottomMargin(int margin) { m_margins.height = margin; }
    
    int getLeftMargin() const { return m_margins.x; }
    int getTopMargin() const { return m_margins.y; }
    int getRightMargin() const { return m_margins.width; }
    int getBottomMargin() const { return m_margins.height; }
    
    Rect getContentsRect() const;
    
    // 尺寸计算
    virtual Size sizeHint() const;
    virtual Size minimumSize() const;
    virtual Size maximumSize() const;
    
    // 布局管理
    void setParentWidget(Widget* widget);
    Widget* parentWidget() const { return m_parentWidget; }
    
    virtual void activate();
    virtual void update();
    virtual void invalidate();
    
    // 布局状态
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);
    
protected:
    // 子类需要实现的核心方法
    virtual void setGeometry(const Rect& rect) = 0;
    virtual Size calculateSizeHint() const = 0;
    virtual Size calculateMinimumSize() const = 0;
    virtual Size calculateMaximumSize() const = 0;
    
    // 工具方法
    Rect alignmentRect(const Rect& rect, const Size& size, Alignment alignment) const;
    void expandingDirections(Orientation* horizontal, Orientation* vertical) const;
    
    // 布局项查找
    LayoutItem* findItemForWidget(Widget* widget) const;
    
private:
    Widget* m_parentWidget = nullptr;
    int m_spacing = 6;
    Rect m_margins{9, 9, 9, 9}; // left, top, right, bottom
    bool m_enabled = true;
    bool m_dirty = true;
    
    void markDirty() { m_dirty = true; }
    void ensureGeometry();
};

// 布局工具函数
namespace LayoutUtils {
    Size expandSize(const Size& size, const Size& hint);
    Size shrinkSize(const Size& size, const Size& hint);
    int distribute(int available, const std::vector<int>& minimums, 
                  const std::vector<int>& stretches);
}

} // namespace QGL

#endif // QGL_LAYOUTS_LAYOUT_H