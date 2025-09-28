#include "ListView.h"
#include "Label.h"
#include "../events/MouseEvent.h"
#include "../events/KeyEvent.h"
#include "../events/FocusEvent.h"
#include "../events/ResizeEvent.h"
#include "../events/ContextMenuEvent.h"
#include "../events/PaintEvent.h"
#include <lvgl.h>
#include <algorithm>
#include <set>

namespace QGL {

// ListViewDelegate实现
ListViewDelegate::ListViewDelegate(Object* parent)
    : Object(parent) {
    setObjectName("ListViewDelegate");
}

Size ListViewDelegate::sizeHint(int index, const Variant& data) const {
    return Size(200, 30);
}

// TextListDelegate实现
TextListDelegate::TextListDelegate(Object* parent)
    : ListViewDelegate(parent) {
    setObjectName("TextListDelegate");
}

Widget* TextListDelegate::createItem(int index, const Variant& data, Widget* parent) {
    Label* label = new Label(parent);
    label->setText(data.toString());
    label->setProperty("padding", "8px");
    label->setProperty("border-bottom", "1px solid #e0e0e0");
    return label;
}

void TextListDelegate::updateItem(int index, const Variant& data, Widget* item) {
    if (Label* label = qobject_cast<Label*>(item)) {
        label->setText(data.toString());
    }
}

Size TextListDelegate::sizeHint(int index, const Variant& data) const {
    return Size(200, 32);
}

// ListModel实现
ListModel::ListModel(Object* parent)
    : Object(parent) {
    setObjectName("ListModel");
}

// StringListModel实现
StringListModel::StringListModel(Object* parent)
    : ListModel(parent) {
    setObjectName("StringListModel");
}

StringListModel::StringListModel(const StringList& strings, Object* parent)
    : ListModel(parent), m_strings(strings) {
    setObjectName("StringListModel");
}

int StringListModel::count() const {
    return m_strings.size();
}

Variant StringListModel::data(int index) const {
    if (index >= 0 && index < m_strings.size()) {
        return Variant(m_strings[index]);
    }
    return Variant();
}

void StringListModel::setData(int index, const Variant& value) {
    if (index >= 0 && index < m_strings.size()) {
        m_strings[index] = value.toString();
        dataChanged.emit(index, index);
    }
}

bool StringListModel::insertItems(int index, int count) {
    if (index < 0 || count <= 0) {
        return false;
    }
    
    index = std::min(index, static_cast<int>(m_strings.size()));
    
    for (int i = 0; i < count; ++i) {
        m_strings.insert(m_strings.begin() + index + i, "");
    }
    
    itemsInserted.emit(index, count);
    return true;
}

bool StringListModel::removeItems(int index, int count) {
    if (index < 0 || count <= 0 || index >= m_strings.size()) {
        return false;
    }
    
    int endIndex = std::min(index + count, static_cast<int>(m_strings.size()));
    count = endIndex - index;
    
    m_strings.erase(m_strings.begin() + index, m_strings.begin() + endIndex);
    
    itemsRemoved.emit(index, count);
    return true;
}

void StringListModel::clear() {
    if (!m_strings.empty()) {
        m_strings.clear();
        modelReset.emit();
    }
}

void StringListModel::setStringList(const StringList& strings) {
    m_strings = strings;
    modelReset.emit();
}

StringList StringListModel::stringList() const {
    return m_strings;
}

void StringListModel::append(const String& string) {
    insert(m_strings.size(), string);
}

void StringListModel::prepend(const String& string) {
    insert(0, string);
}

void StringListModel::insert(int index, const String& string) {
    insertItems(index, 1);
    setData(index, Variant(string));
}

void StringListModel::removeAt(int index) {
    removeItems(index, 1);
}

String StringListModel::at(int index) const {
    return data(index).toString();
}

void StringListModel::setString(int index, const String& string) {
    setData(index, Variant(string));
}

// ListView私有数据类
class ListView::ListViewPrivate {
public:
    std::shared_ptr<ListModel> model;
    std::shared_ptr<ListViewDelegate> delegate;
    
    SelectionMode selectionMode = SelectionMode::SingleSelection;
    int currentIndex = -1;
    std::set<int> selectedIndexes;
    
    bool uniformItemSizes = false;
    Size itemSize = Size(200, 32);
    Size gridSize = Size(100, 100);
    bool gridEnabled = false;
    
    bool dragDropEnabled = false;
    bool editingEnabled = false;
    bool virtualizationEnabled = true;
    int visibleItemsBuffer = 5;
    int itemCacheSize = 100;
    
    bool animationsEnabled = true;
    int insertAnimationDuration = 200;
    int removeAnimationDuration = 150;
    
    // 视图状态
    std::vector<Widget*> visibleItems;
    std::vector<Widget*> recycledItems;
    int firstVisibleIndex = 0;
    int lastVisibleIndex = -1;
    
    // 编辑状态
    int editingIndex = -1;
    Widget* editingWidget = nullptr;
    
    // LVGL相关
    lv_obj_t* lvgl_list = nullptr;
    
    ListViewPrivate() = default;
};

ListView::ListView(Widget* parent)
    : ScrollArea(parent)
    , d_ptr(std::make_unique<ListViewPrivate>()) {
    initializeListView();
}

ListView::~ListView() {
    for (Widget* item : d_ptr->visibleItems) {
        delete item;
    }
    for (Widget* item : d_ptr->recycledItems) {
        delete item;
    }
    
    if (d_ptr->lvgl_list) {
        lv_obj_del(d_ptr->lvgl_list);
    }
}

void ListView::initializeListView() {
    setObjectName("ListView");
    
    d_ptr->delegate = std::make_shared<TextListDelegate>();
    connectDelegateSignals();
    
    d_ptr->lvgl_list = lv_list_create(widget()->getLVGLObject());
    lv_obj_set_size(d_ptr->lvgl_list, LV_PCT(100), LV_PCT(100));
    
    setProperty("background-color", Color::White);
    setProperty("border", "1px solid #e0e0e0");
}

void ListView::setModel(std::shared_ptr<ListModel> model) {
    if (d_ptr->model == model) {
        return;
    }
    
    if (d_ptr->model) {
        disconnectModelSignals();
    }
    
    d_ptr->model = model;
    
    if (d_ptr->model) {
        connectModelSignals();
    }
    
    clearSelection();
    setCurrentIndex(-1);
    updateLayout();
}

std::shared_ptr<ListModel> ListView::model() const {
    return d_ptr->model;
}

void ListView::setDelegate(std::shared_ptr<ListViewDelegate> delegate) {
    if (d_ptr->delegate == delegate) {
        return;
    }
    
    if (d_ptr->delegate) {
        disconnectDelegateSignals();
    }
    
    d_ptr->delegate = delegate;
    
    if (d_ptr->delegate) {
        connectDelegateSignals();
    }
    
    updateLayout();
}

std::shared_ptr<ListViewDelegate> ListView::delegate() const {
    return d_ptr->delegate;
}

void ListView::setSelectionMode(SelectionMode mode) {
    d_ptr->selectionMode = mode;
    
    if (mode == SelectionMode::NoSelection) {
        clearSelection();
    } else if (mode == SelectionMode::SingleSelection && d_ptr->selectedIndexes.size() > 1) {
        int firstSelected = *d_ptr->selectedIndexes.begin();
        clearSelection();
        setIndexSelected(firstSelected, true);
    }
}

SelectionMode ListView::selectionMode() const {
    return d_ptr->selectionMode;
}

int ListView::currentIndex() const {
    return d_ptr->currentIndex;
}

void ListView::setCurrentIndex(int index) {
    if (d_ptr->currentIndex == index) {
        return;
    }
    
    if (d_ptr->model && (index < 0 || index >= d_ptr->model->count())) {
        index = -1;
    }
    
    d_ptr->currentIndex = index;
    
    if (index >= 0 && d_ptr->selectionMode != SelectionMode::NoSelection) {
        if (d_ptr->selectionMode == SelectionMode::SingleSelection) {
            clearSelection();
        }
        setIndexSelected(index, true);
    }
    
    currentIndexChanged.emit(index);
    updateSelection();
}

std::vector<int> ListView::selectedIndexes() const {
    return std::vector<int>(d_ptr->selectedIndexes.begin(), d_ptr->selectedIndexes.end());
}

void ListView::setSelectedIndexes(const std::vector<int>& indexes) {
    clearSelection();
    
    for (int index : indexes) {
        setIndexSelected(index, true);
    }
}

void ListView::selectAll() {
    if (d_ptr->selectionMode == SelectionMode::NoSelection || 
        d_ptr->selectionMode == SelectionMode::SingleSelection) {
        return;
    }
    
    if (!d_ptr->model) {
        return;
    }
    
    for (int i = 0; i < d_ptr->model->count(); ++i) {
        setIndexSelected(i, true);
    }
}

void ListView::clearSelection() {
    if (d_ptr->selectedIndexes.empty()) {
        return;
    }
    
    d_ptr->selectedIndexes.clear();
    selectionChanged.emit();
    updateSelection();
}

Widget* ListView::itemWidget(int index) const {
    if (index < d_ptr->firstVisibleIndex || index > d_ptr->lastVisibleIndex) {
        return nullptr;
    }
    
    int visibleIndex = index - d_ptr->firstVisibleIndex;
    if (visibleIndex >= 0 && visibleIndex < d_ptr->visibleItems.size()) {
        return d_ptr->visibleItems[visibleIndex];
    }
    
    return nullptr;
}

Rect ListView::itemRect(int index) const {
    if (!d_ptr->model || index < 0 || index >= d_ptr->model->count()) {
        return Rect();
    }
    
    Size itemSize = d_ptr->uniformItemSizes ? d_ptr->itemSize : 
                   d_ptr->delegate->sizeHint(index, d_ptr->model->data(index));
    
    int y = index * itemSize.height;
    return Rect(0, y, viewportSize().width, itemSize.height);
}

int ListView::itemAt(const Point& position) const {
    if (!d_ptr->model) {
        return -1;
    }
    
    Point scrolledPos = mapFromViewport(position);
    
    if (d_ptr->uniformItemSizes) {
        int index = scrolledPos.y / d_ptr->itemSize.height;
        return (index >= 0 && index < d_ptr->model->count()) ? index : -1;
    } else {
        int y = 0;
        for (int i = 0; i < d_ptr->model->count(); ++i) {
            Size itemSize = d_ptr->delegate->sizeHint(i, d_ptr->model->data(i));
            if (scrolledPos.y >= y && scrolledPos.y < y + itemSize.height) {
                return i;
            }
            y += itemSize.height;
        }
        return -1;
    }
}

void ListView::scrollTo(int index) {
    if (!d_ptr->model || index < 0 || index >= d_ptr->model->count()) {
        return;
    }
    
    Rect itemRect = this->itemRect(index);
    ensureVisible(itemRect);
}

void ListView::scrollToTop() {
    scrollToPosition(Point(0, 0));
}

void ListView::scrollToBottom() {
    if (!d_ptr->model) {
        return;
    }
    
    int lastIndex = d_ptr->model->count() - 1;
    if (lastIndex >= 0) {
        scrollTo(lastIndex);
    }
}

// 简化的属性设置器和获取器
void ListView::setUniformItemSizes(bool uniform) {
    d_ptr->uniformItemSizes = uniform;
    updateLayout();
}

bool ListView::uniformItemSizes() const {
    return d_ptr->uniformItemSizes;
}

void ListView::setItemSize(const Size& size) {
    d_ptr->itemSize = size;
    if (d_ptr->uniformItemSizes) {
        updateLayout();
    }
}

Size ListView::itemSize() const {
    return d_ptr->itemSize;
}

void ListView::setGridSize(const Size& size) {
    d_ptr->gridSize = size;
    if (d_ptr->gridEnabled) {
        updateLayout();
    }
}

Size ListView::gridSize() const {
    return d_ptr->gridSize;
}

void ListView::setGridEnabled(bool enabled) {
    d_ptr->gridEnabled = enabled;
    updateLayout();
}

bool ListView::isGridEnabled() const {
    return d_ptr->gridEnabled;
}

// 简化的功能实现
void ListView::setDragDropEnabled(bool enabled) {
    d_ptr->dragDropEnabled = enabled;
}

bool ListView::isDragDropEnabled() const {
    return d_ptr->dragDropEnabled;
}

void ListView::setEditingEnabled(bool enabled) {
    d_ptr->editingEnabled = enabled;
}

bool ListView::isEditingEnabled() const {
    return d_ptr->editingEnabled;
}

void ListView::editItem(int index) {
    // 简化实现
}

void ListView::commitEdit() {
    // 简化实现
}

void ListView::cancelEdit() {
    // 简化实现
}

void ListView::setVirtualizationEnabled(bool enabled) {
    d_ptr->virtualizationEnabled = enabled;
    updateLayout();
}

bool ListView::isVirtualizationEnabled() const {
    return d_ptr->virtualizationEnabled;
}

void ListView::setVisibleItemsBuffer(int buffer) {
    d_ptr->visibleItemsBuffer = std::max(0, buffer);
}

int ListView::visibleItemsBuffer() const {
    return d_ptr->visibleItemsBuffer;
}

void ListView::setItemCacheSize(int size) {
    d_ptr->itemCacheSize = std::max(0, size);
}

int ListView::itemCacheSize() const {
    return d_ptr->itemCacheSize;
}

void ListView::setAnimationsEnabled(bool enabled) {
    d_ptr->animationsEnabled = enabled;
}

bool ListView::areAnimationsEnabled() const {
    return d_ptr->animationsEnabled;
}

void ListView::setInsertAnimationDuration(int duration) {
    d_ptr->insertAnimationDuration = std::max(0, duration);
}

int ListView::insertAnimationDuration() const {
    return d_ptr->insertAnimationDuration;
}

void ListView::setRemoveAnimationDuration(int duration) {
    d_ptr->removeAnimationDuration = std::max(0, duration);
}

int ListView::removeAnimationDuration() const {
    return d_ptr->removeAnimationDuration;
}

// 简化的事件处理
void ListView::resizeEvent(ResizeEvent* event) {
    ScrollArea::resizeEvent(event);
    updateViewport();
}

void ListView::mousePressEvent(MouseEvent* event) {
    int index = itemAt(event->position());
    if (index >= 0) {
        setCurrentIndex(index);
        itemPressed.emit(index);
    }
    
    ScrollArea::mousePressEvent(event);
}

void ListView::mouseMoveEvent(MouseEvent* event) {
    ScrollArea::mouseMoveEvent(event);
}

void ListView::mouseReleaseEvent(MouseEvent* event) {
    int index = itemAt(event->position());
    if (index >= 0) {
        itemClicked.emit(index);
        onItemClicked(index);
    }
    
    ScrollArea::mouseReleaseEvent(event);
}

void ListView::mouseDoubleClickEvent(MouseEvent* event) {
    int index = itemAt(event->position());
    if (index >= 0) {
        itemDoubleClicked.emit(index);
        itemActivated.emit(index);
        onItemDoubleClicked(index);
    }
}

void ListView::keyPressEvent(KeyEvent* event) {
    if (!d_ptr->model) {
        ScrollArea::keyPressEvent(event);
        return;
    }
    
    int newIndex = d_ptr->currentIndex;
    
    switch (event->key()) {
        case Key::Up:
            newIndex = std::max(0, d_ptr->currentIndex - 1);
            break;
        case Key::Down:
            newIndex = std::min(d_ptr->model->count() - 1, d_ptr->currentIndex + 1);
            break;
        case Key::Home:
            newIndex = 0;
            break;
        case Key::End:
            newIndex = d_ptr->model->count() - 1;
            break;
        default:
            ScrollArea::keyPressEvent(event);
            return;
    }
    
    if (newIndex != d_ptr->currentIndex) {
        setCurrentIndex(newIndex);
        scrollTo(newIndex);
    }
    
    event->accept();
}

void ListView::focusInEvent(FocusEvent* event) {
    ScrollArea::focusInEvent(event);
    updateSelection();
}

void ListView::focusOutEvent(FocusEvent* event) {
    ScrollArea::focusOutEvent(event);
}

void ListView::contextMenuEvent(ContextMenuEvent* event) {
    int index = itemAt(event->position());
    contextMenuRequested.emit(index, event->position());
}

// 内部方法的简化实现
void ListView::updateViewport() {
    updateLayout();
}

void ListView::updateLayout() {
    if (!d_ptr->model || !d_ptr->delegate) {
        return;
    }
    
    // 清理现有项目
    for (Widget* item : d_ptr->visibleItems) {
        delete item;
    }
    d_ptr->visibleItems.clear();
    
    // 创建项目（简化版本，不支持虚拟化）
    for (int i = 0; i < d_ptr->model->count(); ++i) {
        Widget* item = createItemWidget(i);
        if (item) {
            d_ptr->visibleItems.push_back(item);
            
            // 设置项目位置
            Rect itemRect = this->itemRect(i);
            item->setGeometry(itemRect);
        }
    }
    
    updateSelection();
}

void ListView::updateSelection() {
    for (int i = 0; i < d_ptr->visibleItems.size(); ++i) {
        Widget* item = d_ptr->visibleItems[i];
        int index = i;
        
        bool selected = isIndexSelected(index);
        bool current = (index == d_ptr->currentIndex);
        
        if (selected) {
            item->setProperty("background-color", Color(0, 120, 215, 100));
        } else {
            item->setProperty("background-color", Color::Transparent);
        }
        
        if (current && hasFocus()) {
            item->setProperty("border", "2px solid #0078d4");
        } else {
            item->setProperty("border", "none");
        }
    }
}

Widget* ListView::createItemWidget(int index) {
    if (!d_ptr->model || !d_ptr->delegate) {
        return nullptr;
    }
    
    Variant data = d_ptr->model->data(index);
    return d_ptr->delegate->createItem(index, data, this);
}

void ListView::scrollContentsBy(int dx, int dy) {
    ScrollArea::scrollContentsBy(dx, dy);
}

bool ListView::isIndexSelected(int index) const {
    return d_ptr->selectedIndexes.find(index) != d_ptr->selectedIndexes.end();
}

void ListView::setIndexSelected(int index, bool selected) {
    if (!d_ptr->model || index < 0 || index >= d_ptr->model->count()) {
        return;
    }
    
    bool wasSelected = isIndexSelected(index);
    
    if (selected && !wasSelected) {
        d_ptr->selectedIndexes.insert(index);
        selectionChanged.emit();
    } else if (!selected && wasSelected) {
        d_ptr->selectedIndexes.erase(index);
        selectionChanged.emit();
    }
}

// 信号连接方法（简化实现）
void ListView::connectModelSignals() {
    if (!d_ptr->model) return;
    
    d_ptr->model->dataChanged.connect([this](int start, int end) { onDataChanged(start, end); });
    d_ptr->model->itemsInserted.connect([this](int index, int count) { onItemsInserted(index, count); });
    d_ptr->model->itemsRemoved.connect([this](int index, int count) { onItemsRemoved(index, count); });
    d_ptr->model->modelReset.connect([this]() { onModelReset(); });
    d_ptr->model->layoutChanged.connect([this]() { onLayoutChanged(); });
}

void ListView::disconnectModelSignals() {
    // 简化实现
}

void ListView::connectDelegateSignals() {
    if (!d_ptr->delegate) return;
    
    d_ptr->delegate->itemClicked.connect([this](int index) { onItemClicked(index); });
    d_ptr->delegate->itemDoubleClicked.connect([this](int index) { onItemDoubleClicked(index); });
    d_ptr->delegate->itemChanged.connect([this](int index, const Variant& data) { onItemChanged(index, data); });
}

void ListView::disconnectDelegateSignals() {
    // 简化实现
}

// 模型事件处理
void ListView::onDataChanged(int startIndex, int endIndex) {
    updateLayout();
}

void ListView::onItemsInserted(int index, int count) {
    updateLayout();
}

void ListView::onItemsRemoved(int index, int count) {
    updateLayout();
}

void ListView::onModelReset() {
    clearSelection();
    setCurrentIndex(-1);
    updateLayout();
}

void ListView::onLayoutChanged() {
    updateLayout();
}

void ListView::onItemClicked(int index) {
    // 处理项目点击
}

void ListView::onItemDoubleClicked(int index) {
    // 处理项目双击
}

void ListView::onItemChanged(int index, const Variant& data) {
    if (d_ptr->model) {
        d_ptr->model->setData(index, data);
    }
}

} // namespace QGL