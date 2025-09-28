#pragma once

#include "ScrollArea.h"
#include "../core/Types.h"
#include "../signals/Signal.h"
#include "../properties/Property.h"
#include <memory>
#include <functional>
#include <vector>

namespace QGL {

// 前向声明
class ListModel;
class ListViewDelegate;
class ListItem;

/**
 * @brief 列表选择模式
 */
enum class SelectionMode {
    NoSelection,        // 不可选择
    SingleSelection,    // 单选
    MultiSelection,     // 多选
    ExtendedSelection   // 扩展选择（支持Ctrl/Shift）
};

/**
 * @brief 列表滚动模式
 */
enum class ScrollMode {
    ScrollPerItem,      // 按项目滚动
    ScrollPerPixel      // 按像素滚动
};

/**
 * @brief 列表项委托基类
 * 
 * ListViewDelegate负责创建和配置列表项的显示。
 * 这是适配器模式的核心组件。
 */
class ListViewDelegate : public Object {
    QGL_OBJECT(ListViewDelegate)

public:
    explicit ListViewDelegate(Object* parent = nullptr);
    virtual ~ListViewDelegate() = default;

    // 创建列表项控件
    virtual Widget* createItem(int index, const Variant& data, Widget* parent) = 0;
    
    // 更新列表项内容
    virtual void updateItem(int index, const Variant& data, Widget* item) = 0;
    
    // 项目大小提示
    virtual Size sizeHint(int index, const Variant& data) const;
    
    // 项目类型（用于优化回收）
    virtual int itemType(int index, const Variant& data) const { return 0; }
    
    // 是否可编辑
    virtual bool isEditable(int index) const { return false; }
    
    // 开始编辑
    virtual Widget* createEditor(int index, const Variant& data, Widget* parent) { return nullptr; }
    virtual void setEditorData(int index, const Variant& data, Widget* editor) {}
    virtual Variant getEditorData(int index, Widget* editor) const { return Variant(); }
    
    // 自定义绘制（可选）
    virtual void paint(int index, const Variant& data, Widget* item, PaintEvent* event) {}

    // 信号
    SIGNAL(itemClicked, int);               // 项目被点击
    SIGNAL(itemDoubleClicked, int);         // 项目被双击
    SIGNAL(itemChanged, int, const Variant&); // 项目数据改变
};

/**
 * @brief 简单文本列表委托
 */
class TextListDelegate : public ListViewDelegate {
    QGL_OBJECT(TextListDelegate)

public:
    explicit TextListDelegate(Object* parent = nullptr);
    
    Widget* createItem(int index, const Variant& data, Widget* parent) override;
    void updateItem(int index, const Variant& data, Widget* item) override;
    Size sizeHint(int index, const Variant& data) const override;
};

/**
 * @brief 列表数据模型基类
 * 
 * ListModel提供数据源接口，支持数据的增删改查。
 */
class ListModel : public Object {
    QGL_OBJECT(ListModel)

public:
    explicit ListModel(Object* parent = nullptr);
    virtual ~ListModel() = default;

    // 数据访问
    virtual int count() const = 0;
    virtual Variant data(int index) const = 0;
    virtual void setData(int index, const Variant& value) = 0;
    
    // 数据修改
    virtual bool insertItems(int index, int count) = 0;
    virtual bool removeItems(int index, int count) = 0;
    virtual void clear() = 0;
    
    // 排序和过滤
    virtual void sort(int column = 0, bool ascending = true) {}
    virtual void setFilter(const String& filter) {}
    
    // 角色数据（用于不同显示需求）
    virtual Variant data(int index, int role) const { return data(index); }
    virtual void setData(int index, const Variant& value, int role) { setData(index, value); }

    // 信号
    SIGNAL(dataChanged, int, int);          // 数据改变 (startIndex, endIndex)
    SIGNAL(itemsInserted, int, int);        // 项目插入 (index, count)
    SIGNAL(itemsRemoved, int, int);         // 项目移除 (index, count)
    SIGNAL(modelReset);                     // 模型重置
    SIGNAL(layoutChanged);                  // 布局改变
};

/**
 * @brief 简单字符串列表模型
 */
class StringListModel : public ListModel {
    QGL_OBJECT(StringListModel)

public:
    explicit StringListModel(Object* parent = nullptr);
    explicit StringListModel(const StringList& strings, Object* parent = nullptr);
    
    // ListModel接口实现
    int count() const override;
    Variant data(int index) const override;
    void setData(int index, const Variant& value) override;
    
    bool insertItems(int index, int count) override;
    bool removeItems(int index, int count) override;
    void clear() override;
    
    // 便利方法
    void setStringList(const StringList& strings);
    StringList stringList() const;
    
    void append(const String& string);
    void prepend(const String& string);
    void insert(int index, const String& string);
    void removeAt(int index);
    
    String at(int index) const;
    void setString(int index, const String& string);

private:
    StringList m_strings;
};

/**
 * @brief 列表视图控件
 * 
 * ListView使用模型-视图-委托架构，提供高效的大数据量显示能力。
 * 支持虚拟化、选择、编辑、排序等功能。
 */
class ListView : public ScrollArea {
    QGL_OBJECT(ListView)

public:
    explicit ListView(Widget* parent = nullptr);
    ~ListView() override;

    // 模型和委托
    void setModel(std::shared_ptr<ListModel> model);
    std::shared_ptr<ListModel> model() const;
    
    void setDelegate(std::shared_ptr<ListViewDelegate> delegate);
    std::shared_ptr<ListViewDelegate> delegate() const;
    
    // 选择模式
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode() const;
    
    // 当前选择
    int currentIndex() const;
    void setCurrentIndex(int index);
    
    std::vector<int> selectedIndexes() const;
    void setSelectedIndexes(const std::vector<int>& indexes);
    
    void selectAll();
    void clearSelection();
    
    // 项目访问
    Widget* itemWidget(int index) const;
    Rect itemRect(int index) const;
    int itemAt(const Point& position) const;
    
    // 滚动到指定项目
    void scrollTo(int index);
    void scrollToTop();
    void scrollToBottom();
    
    // 项目大小
    void setUniformItemSizes(bool uniform);
    bool uniformItemSizes() const;
    
    void setItemSize(const Size& size);
    Size itemSize() const;
    
    // 网格模式
    void setGridSize(const Size& size);
    Size gridSize() const;
    
    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;
    
    // 拖拽支持
    void setDragDropEnabled(bool enabled);
    bool isDragDropEnabled() const;
    
    // 编辑支持
    void setEditingEnabled(bool enabled);
    bool isEditingEnabled() const;
    
    void editItem(int index);
    void commitEdit();
    void cancelEdit();
    
    // 虚拟化（用于大数据量优化）
    void setVirtualizationEnabled(bool enabled);
    bool isVirtualizationEnabled() const;
    
    void setVisibleItemsBuffer(int buffer);
    int visibleItemsBuffer() const;
    
    // 性能优化
    void setItemCacheSize(int size);
    int itemCacheSize() const;
    
    // 动画效果
    void setAnimationsEnabled(bool enabled);
    bool areAnimationsEnabled() const;
    
    void setInsertAnimationDuration(int duration);
    int insertAnimationDuration() const;
    
    void setRemoveAnimationDuration(int duration);
    int removeAnimationDuration() const;

    // 信号
    SIGNAL(currentIndexChanged, int);           // 当前索引改变
    SIGNAL(selectionChanged);                   // 选择改变
    SIGNAL(itemClicked, int);                   // 项目点击
    SIGNAL(itemDoubleClicked, int);             // 项目双击
    SIGNAL(itemActivated, int);                 // 项目激活
    SIGNAL(itemEntered, int);                   // 鼠标进入项目
    SIGNAL(itemPressed, int);                   // 项目按下
    SIGNAL(contextMenuRequested, int, const Point&); // 上下文菜单请求

protected:
    // 事件处理
    void resizeEvent(ResizeEvent* event) override;
    void mousePressEvent(MouseEvent* event) override;
    void mouseMoveEvent(MouseEvent* event) override;
    void mouseReleaseEvent(MouseEvent* event) override;
    void mouseDoubleClickEvent(MouseEvent* event) override;
    void keyPressEvent(KeyEvent* event) override;
    void focusInEvent(FocusEvent* event) override;
    void focusOutEvent(FocusEvent* event) override;
    void contextMenuEvent(ContextMenuEvent* event) override;
    
    // 内部方法
    virtual void updateViewport();
    virtual void updateLayout();
    virtual void updateSelection();
    
    // 项目管理
    void createVisibleItems();
    void updateVisibleItems();
    void recycleItem(Widget* item);
    Widget* createItemWidget(int index);
    
    // 滚动处理
    virtual void scrollContentsBy(int dx, int dy) override;
    
    // 选择处理
    void updateSelectionForIndex(int index, bool extend = false);
    bool isIndexSelected(int index) const;
    void setIndexSelected(int index, bool selected);

private:
    class ListViewPrivate;
    std::unique_ptr<ListViewPrivate> d_ptr;
    
    void initializeListView();
    void connectModelSignals();
    void disconnectModelSignals();
    void connectDelegateSignals();
    void disconnectDelegateSignals();
    
    // 模型更新处理
    void onDataChanged(int startIndex, int endIndex);
    void onItemsInserted(int index, int count);
    void onItemsRemoved(int index, int count);
    void onModelReset();
    void onLayoutChanged();
    
    // 委托事件处理
    void onItemClicked(int index);
    void onItemDoubleClicked(int index);
    void onItemChanged(int index, const Variant& data);
    
    // 属性声明
    QGL_PROPERTY(SelectionMode, selectionMode, SELECTION_MODE)
    QGL_PROPERTY(int, currentIndex, CURRENT_INDEX)
    QGL_PROPERTY(bool, uniformItemSizes, UNIFORM_ITEM_SIZES)
    QGL_PROPERTY(Size, itemSize, ITEM_SIZE)
    QGL_PROPERTY(Size, gridSize, GRID_SIZE)
    QGL_PROPERTY(bool, gridEnabled, GRID_ENABLED)
    QGL_PROPERTY(bool, dragDropEnabled, DRAG_DROP_ENABLED)
    QGL_PROPERTY(bool, editingEnabled, EDITING_ENABLED)
    QGL_PROPERTY(bool, virtualizationEnabled, VIRTUALIZATION_ENABLED)
    QGL_PROPERTY(int, visibleItemsBuffer, VISIBLE_ITEMS_BUFFER)
    QGL_PROPERTY(int, itemCacheSize, ITEM_CACHE_SIZE)
    QGL_PROPERTY(bool, animationsEnabled, ANIMATIONS_ENABLED)
    QGL_PROPERTY(int, insertAnimationDuration, INSERT_ANIMATION_DURATION)
    QGL_PROPERTY(int, removeAnimationDuration, REMOVE_ANIMATION_DURATION)
};

} // namespace QGL