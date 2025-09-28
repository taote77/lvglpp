#include "LineEdit.h"
#include "../core/Application.h"
#include "../events/KeyEvent.h"
#include "../events/MouseEvent.h"
#include "../events/FocusEvent.h"
#include "../events/PaintEvent.h"
#include "../events/ResizeEvent.h"
#include "../events/ContextMenuEvent.h"
#include "../events/DragEvent.h"
#include <lvgl.h>
#include <algorithm>
#include <cctype>
#include <cmath>

namespace QGL {

// IntValidator实现
IntValidator::IntValidator(int bottom, int top) 
    : m_bottom(bottom), m_top(top) {
}

bool IntValidator::validate(const String& text) const {
    if (text.isEmpty()) {
        return true; // 空字符串被认为是有效的
    }
    
    try {
        int value = std::stoi(text.toStdString());
        return value >= m_bottom && value <= m_top;
    } catch (const std::exception&) {
        return false;
    }
}

String IntValidator::fixup(const String& text) const {
    if (text.isEmpty()) {
        return text;
    }
    
    try {
        int value = std::stoi(text.toStdString());
        value = std::max(m_bottom, std::min(m_top, value));
        return String::number(value);
    } catch (const std::exception&) {
        return String::number(m_bottom);
    }
}

void IntValidator::setRange(int bottom, int top) {
    m_bottom = bottom;
    m_top = top;
}

// DoubleValidator实现
DoubleValidator::DoubleValidator(double bottom, double top, int decimals)
    : m_bottom(bottom), m_top(top), m_decimals(decimals) {
}

bool DoubleValidator::validate(const String& text) const {
    if (text.isEmpty()) {
        return true;
    }
    
    try {
        double value = std::stod(text.toStdString());
        return value >= m_bottom && value <= m_top;
    } catch (const std::exception&) {
        return false;
    }
}

String DoubleValidator::fixup(const String& text) const {
    if (text.isEmpty()) {
        return text;
    }
    
    try {
        double value = std::stod(text.toStdString());
        value = std::max(m_bottom, std::min(m_top, value));
        
        if (m_decimals < 1000) {
            // 限制小数位数
            double factor = std::pow(10.0, m_decimals);
            value = std::round(value * factor) / factor;
        }
        
        return String::number(value);
    } catch (const std::exception&) {
        return String::number(m_bottom);
    }
}

void DoubleValidator::setRange(double bottom, double top, int decimals) {
    m_bottom = bottom;
    m_top = top;
    m_decimals = decimals;
}

// RegExpValidator实现
RegExpValidator::RegExpValidator(const String& pattern) 
    : m_pattern(pattern) {
    try {
        m_regExp = std::regex(pattern.toStdString());
    } catch (const std::exception&) {
        // 如果正则表达式无效，使用匹配所有的表达式
        m_regExp = std::regex(".*");
    }
}

bool RegExpValidator::validate(const String& text) const {
    try {
        return std::regex_match(text.toStdString(), m_regExp);
    } catch (const std::exception&) {
        return false;
    }
}

void RegExpValidator::setRegExp(const String& pattern) {
    m_pattern = pattern;
    try {
        m_regExp = std::regex(pattern.toStdString());
    } catch (const std::exception&) {
        m_regExp = std::regex(".*");
    }
}

// LineEdit私有数据类
class LineEdit::LineEditPrivate {
public:
    String text;
    String placeholderText;
    String displayText;
    String inputMask;
    
    bool readOnly = false;
    bool modified = false;
    bool undoRedoEnabled = true;
    bool clearButtonEnabled = false;
    bool frame = true;
    bool dragEnabled = false;
    
    EchoMode echoMode = EchoMode::Normal;
    int maxLength = 32767;
    int cursorPos = 0;
    int selectionStart = -1;
    int selectionLength = 0;
    
    TextAlignment alignment = TextAlignment::Left;
    FocusPolicy focusPolicy = FocusPolicy::StrongFocus;
    ContextMenuPolicy contextMenuPolicy = ContextMenuPolicy::DefaultContextMenu;
    
    Rect textMargins = {0, 0, 0, 0};
    
    std::shared_ptr<Validator> validator;
    std::vector<String> completions;
    
    // 撤销/重做历史
    std::vector<String> undoStack;
    std::vector<String> redoStack;
    int maxUndoSteps = 100;
    
    // LVGL相关
    lv_obj_t* lvgl_textarea = nullptr;
    
    // 内部状态
    bool internalTextChange = false;
    bool cursorVisible = true;
    
    LineEditPrivate() = default;
};

LineEdit::LineEdit(Widget* parent)
    : Widget(parent)
    , d_ptr(std::make_unique<LineEditPrivate>()) {
    initializeLineEdit();
}

LineEdit::LineEdit(const String& text, Widget* parent)
    : Widget(parent)
    , d_ptr(std::make_unique<LineEditPrivate>()) {
    initializeLineEdit();
    setText(text);
}

LineEdit::~LineEdit() {
    if (d_ptr->lvgl_textarea) {
        lv_obj_del(d_ptr->lvgl_textarea);
    }
}

void LineEdit::initializeLineEdit() {
    setObjectName("LineEdit");
    setupLVGLTextArea();
    connectLVGLSignals();
    
    // 设置默认样式
    setProperty("background-color", Color(255, 255, 255));
    setProperty("border", "1px solid #cccccc");
    setProperty("border-radius", "4px");
    setProperty("padding", "8px");
    setProperty("font-size", "14px");
    setProperty("color", Color(0, 0, 0));
}

void LineEdit::setupLVGLTextArea() {
    d_ptr->lvgl_textarea = lv_textarea_create(getLVGLObject());
    
    // 设置为单行模式
    lv_textarea_set_one_line(d_ptr->lvgl_textarea, true);
    
    // 设置默认样式
    lv_obj_set_style_bg_color(d_ptr->lvgl_textarea, lv_color_white(), 0);
    lv_obj_set_style_border_color(d_ptr->lvgl_textarea, lv_color_hex(0xcccccc), 0);
    lv_obj_set_style_border_width(d_ptr->lvgl_textarea, 1, 0);
    lv_obj_set_style_radius(d_ptr->lvgl_textarea, 4, 0);
    lv_obj_set_style_pad_all(d_ptr->lvgl_textarea, 8, 0);
    
    // 设置大小
    lv_obj_set_size(d_ptr->lvgl_textarea, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_align(d_ptr->lvgl_textarea, LV_ALIGN_CENTER, 0, 0);
}

void LineEdit::connectLVGLSignals() {
    lv_obj_add_event_cb(d_ptr->lvgl_textarea, onTextChanged, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(d_ptr->lvgl_textarea, onInsert, LV_EVENT_INSERT, this);
    lv_obj_add_event_cb(d_ptr->lvgl_textarea, onReady, LV_EVENT_READY, this);
    lv_obj_add_event_cb(d_ptr->lvgl_textarea, onCancel, LV_EVENT_CANCEL, this);
}

// 文本内容
void LineEdit::setText(const String& text) {
    if (d_ptr->text == text) {
        return;
    }
    
    String newText = text;
    
    // 应用最大长度限制
    if (newText.length() > d_ptr->maxLength) {
        newText = newText.left(d_ptr->maxLength);
    }
    
    // 验证输入
    if (d_ptr->validator && !d_ptr->validator->validate(newText)) {
        newText = d_ptr->validator->fixup(newText);
    }
    
    // 应用输入掩码
    if (!d_ptr->inputMask.isEmpty()) {
        newText = maskString(newText);
    }
    
    d_ptr->text = newText;
    d_ptr->modified = true;
    
    // 更新显示文本
    updateDisplay();
    
    // 更新LVGL控件
    if (d_ptr->lvgl_textarea) {
        d_ptr->internalTextChange = true;
        lv_textarea_set_text(d_ptr->lvgl_textarea, d_ptr->displayText.toUtf8().data());
        d_ptr->internalTextChange = false;
    }
    
    // 重置光标位置
    setCursorPosition(newText.length());
    
    // 发射信号
    textChanged.emit(d_ptr->text);
}

String LineEdit::text() const {
    return d_ptr->text;
}

String LineEdit::displayText() const {
    return d_ptr->displayText;
}

void LineEdit::setPlaceholderText(const String& text) {
    d_ptr->placeholderText = text;
    
    if (d_ptr->lvgl_textarea) {
        lv_textarea_set_placeholder_text(d_ptr->lvgl_textarea, text.toUtf8().data());
    }
}

String LineEdit::placeholderText() const {
    return d_ptr->placeholderText;
}

// 基本的选中文本实现
void LineEdit::selectAll() {
    setSelection(0, d_ptr->text.length());
}

void LineEdit::deselect() {
    d_ptr->selectionStart = -1;
    d_ptr->selectionLength = 0;
    updateSelection();
    selectionChanged.emit();
}

bool LineEdit::hasSelectedText() const {
    return d_ptr->selectionLength > 0;
}

String LineEdit::selectedText() const {
    if (!hasSelectedText()) {
        return String();
    }
    
    return d_ptr->text.mid(d_ptr->selectionStart, d_ptr->selectionLength);
}

int LineEdit::selectionStart() const {
    return d_ptr->selectionStart;
}

int LineEdit::selectionLength() const {
    return d_ptr->selectionLength;
}

void LineEdit::setSelection(int start, int length) {
    int textLength = d_ptr->text.length();
    start = std::max(0, std::min(start, textLength));
    length = std::max(0, std::min(length, textLength - start));
    
    d_ptr->selectionStart = start;
    d_ptr->selectionLength = length;
    
    updateSelection();
    selectionChanged.emit();
}

// 光标位置控制的基本实现
void LineEdit::setCursorPosition(int position) {
    int oldPos = d_ptr->cursorPos;
    d_ptr->cursorPos = std::max(0, std::min(position, d_ptr->text.length()));
    
    updateCursor();
    
    if (oldPos != d_ptr->cursorPos) {
        cursorPositionChanged.emit(oldPos, d_ptr->cursorPos);
    }
}

int LineEdit::cursorPosition() const {
    return d_ptr->cursorPos;
}

// 简化的光标移动实现
void LineEdit::cursorForward(bool mark, int steps) {
    int newPos = std::min(d_ptr->cursorPos + steps, d_ptr->text.length());
    setCursorPosition(newPos);
}

void LineEdit::cursorBackward(bool mark, int steps) {
    cursorForward(mark, -steps);
}

void LineEdit::cursorWordForward(bool mark) {
    cursorForward(mark, 1); // 简化实现
}

void LineEdit::cursorWordBackward(bool mark) {
    cursorBackward(mark, 1); // 简化实现
}

void LineEdit::home(bool mark) {
    setCursorPosition(0);
}

void LineEdit::end(bool mark) {
    setCursorPosition(d_ptr->text.length());
}

// 编辑操作的基本实现
void LineEdit::clear() {
    setText("");
}

void LineEdit::copy() const {
    // 简化实现
}

void LineEdit::cut() {
    if (d_ptr->readOnly) {
        return;
    }
    // 简化实现
}

void LineEdit::paste() {
    if (d_ptr->readOnly) {
        return;
    }
    // 简化实现
}

void LineEdit::undo() {
    // 简化实现
}

void LineEdit::redo() {
    // 简化实现
}

void LineEdit::insert(const String& text) {
    if (d_ptr->readOnly || text.isEmpty()) {
        return;
    }
    
    String newText = d_ptr->text;
    newText.insert(d_ptr->cursorPos, text);
    
    if (!validateInput(newText)) {
        inputRejected.emit();
        return;
    }
    
    setText(newText);
    setCursorPosition(d_ptr->cursorPos + text.length());
    textEdited.emit(d_ptr->text);
}

void LineEdit::del() {
    if (d_ptr->readOnly) {
        return;
    }
    
    if (d_ptr->cursorPos < d_ptr->text.length()) {
        String newText = d_ptr->text;
        newText.remove(d_ptr->cursorPos, 1);
        setText(newText);
        textEdited.emit(d_ptr->text);
    }
}

void LineEdit::backspace() {
    if (d_ptr->readOnly) {
        return;
    }
    
    if (d_ptr->cursorPos > 0) {
        String newText = d_ptr->text;
        newText.remove(d_ptr->cursorPos - 1, 1);
        setCursorPosition(d_ptr->cursorPos - 1);
        setText(newText);
        textEdited.emit(d_ptr->text);
    }
}

// 属性设置器和获取器
void LineEdit::setReadOnly(bool readOnly) {
    d_ptr->readOnly = readOnly;
    
    if (d_ptr->lvgl_textarea) {
        if (readOnly) {
            lv_obj_add_state(d_ptr->lvgl_textarea, LV_STATE_DISABLED);
        } else {
            lv_obj_clear_state(d_ptr->lvgl_textarea, LV_STATE_DISABLED);
        }
    }
    
    updateDisplay();
}

bool LineEdit::isReadOnly() const {
    return d_ptr->readOnly;
}

void LineEdit::setEchoMode(EchoMode mode) {
    d_ptr->echoMode = mode;
    updateDisplay();
    
    if (d_ptr->lvgl_textarea) {
        lv_textarea_set_password_mode(d_ptr->lvgl_textarea, 
                                    mode == EchoMode::Password || mode == EchoMode::PasswordEchoOnEdit);
    }
}

EchoMode LineEdit::echoMode() const {
    return d_ptr->echoMode;
}

void LineEdit::setInputMask(const String& mask) {
    d_ptr->inputMask = mask;
    updateDisplay();
}

String LineEdit::inputMask() const {
    return d_ptr->inputMask;
}

void LineEdit::setMaxLength(int length) {
    d_ptr->maxLength = std::max(0, length);
    
    if (d_ptr->text.length() > d_ptr->maxLength) {
        setText(d_ptr->text.left(d_ptr->maxLength));
    }
    
    if (d_ptr->lvgl_textarea) {
        lv_textarea_set_max_length(d_ptr->lvgl_textarea, d_ptr->maxLength);
    }
}

int LineEdit::maxLength() const {
    return d_ptr->maxLength;
}

void LineEdit::setValidator(std::shared_ptr<Validator> validator) {
    d_ptr->validator = validator;
    
    // 重新验证当前文本
    if (validator && !validator->validate(d_ptr->text)) {
        setText(validator->fixup(d_ptr->text));
    }
}

std::shared_ptr<Validator> LineEdit::validator() const {
    return d_ptr->validator;
}

bool LineEdit::hasAcceptableInput() const {
    if (!d_ptr->validator) {
        return true;
    }
    
    return d_ptr->validator->validate(d_ptr->text);
}

// 其他属性的简化实现
void LineEdit::setCompleter(const std::vector<String>& completions) {
    d_ptr->completions = completions;
}

std::vector<String> LineEdit::completer() const {
    return d_ptr->completions;
}

void LineEdit::clearCompleter() {
    d_ptr->completions.clear();
}

void LineEdit::setDragEnabled(bool enabled) {
    d_ptr->dragEnabled = enabled;
}

bool LineEdit::isDragEnabled() const {
    return d_ptr->dragEnabled;
}

void LineEdit::setAlignment(TextAlignment alignment) {
    d_ptr->alignment = alignment;
    
    if (d_ptr->lvgl_textarea) {
        lv_text_align_t lvAlign = LV_TEXT_ALIGN_LEFT;
        switch (alignment) {
            case TextAlignment::Left:
                lvAlign = LV_TEXT_ALIGN_LEFT;
                break;
            case TextAlignment::Center:
                lvAlign = LV_TEXT_ALIGN_CENTER;
                break;
            case TextAlignment::Right:
                lvAlign = LV_TEXT_ALIGN_RIGHT;
                break;
            default:
                break;
        }
        lv_obj_set_style_text_align(d_ptr->lvgl_textarea, lvAlign, 0);
    }
}

TextAlignment LineEdit::alignment() const {
    return d_ptr->alignment;
}

void LineEdit::setTextMargins(int left, int top, int right, int bottom) {
    d_ptr->textMargins = Rect(left, top, right - left, bottom - top);
    
    if (d_ptr->lvgl_textarea) {
        lv_obj_set_style_pad_left(d_ptr->lvgl_textarea, left, 0);
        lv_obj_set_style_pad_top(d_ptr->lvgl_textarea, top, 0);
        lv_obj_set_style_pad_right(d_ptr->lvgl_textarea, right, 0);
        lv_obj_set_style_pad_bottom(d_ptr->lvgl_textarea, bottom, 0);
    }
}

void LineEdit::setTextMargins(const Rect& margins) {
    setTextMargins(margins.x(), margins.y(), 
                  margins.x() + margins.width(), 
                  margins.y() + margins.height());
}

Rect LineEdit::textMargins() const {
    return d_ptr->textMargins;
}

// Widget接口重写
void LineEdit::setEnabled(bool enabled) {
    Widget::setEnabled(enabled);
    
    if (d_ptr->lvgl_textarea) {
        if (enabled) {
            lv_obj_clear_state(d_ptr->lvgl_textarea, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(d_ptr->lvgl_textarea, LV_STATE_DISABLED);
        }
    }
}

void LineEdit::setVisible(bool visible) {
    Widget::setVisible(visible);
    
    if (d_ptr->lvgl_textarea) {
        if (visible) {
            lv_obj_clear_flag(d_ptr->lvgl_textarea, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(d_ptr->lvgl_textarea, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void LineEdit::setFocus() {
    Widget::setFocus();
    
    if (d_ptr->lvgl_textarea) {
        lv_obj_add_state(d_ptr->lvgl_textarea, LV_STATE_FOCUSED);
        lv_group_focus_obj(d_ptr->lvgl_textarea);
    }
}

void LineEdit::clearFocus() {
    Widget::clearFocus();
    
    if (d_ptr->lvgl_textarea) {
        lv_obj_clear_state(d_ptr->lvgl_textarea, LV_STATE_FOCUSED);
    }
    
    editingFinished.emit();
}

bool LineEdit::hasFocus() const {
    if (d_ptr->lvgl_textarea) {
        return lv_obj_has_state(d_ptr->lvgl_textarea, LV_STATE_FOCUSED);
    }
    return Widget::hasFocus();
}

// 内部方法的简化实现
void LineEdit::updateDisplay() {
    switch (d_ptr->echoMode) {
        case EchoMode::Normal:
            d_ptr->displayText = d_ptr->text;
            break;
        case EchoMode::NoEcho:
            d_ptr->displayText = "";
            break;
        case EchoMode::Password:
        case EchoMode::PasswordEchoOnEdit:
            d_ptr->displayText = String('*').repeated(d_ptr->text.length());
            break;
    }
    
    update();
}

void LineEdit::updateCursor() {
    if (d_ptr->lvgl_textarea) {
        lv_textarea_set_cursor_pos(d_ptr->lvgl_textarea, d_ptr->cursorPos);
    }
}

void LineEdit::updateSelection() {
    // 简化实现
}

String LineEdit::maskString(const String& text) const {
    // 简化实现，直接返回原文本
    return text;
}

bool LineEdit::validateInput(const String& text) const {
    if (d_ptr->validator) {
        return d_ptr->validator->isAcceptableInput(text);
    }
    return true;
}

String LineEdit::fixupInput(const String& text) const {
    if (d_ptr->validator) {
        return d_ptr->validator->fixup(text);
    }
    return text;
}

// 简化的焦点和属性实现
void LineEdit::setFocusPolicy(FocusPolicy policy) {
    d_ptr->focusPolicy = policy;
}

FocusPolicy LineEdit::focusPolicy() const {
    return d_ptr->focusPolicy;
}

void LineEdit::setClearButtonEnabled(bool enabled) {
    d_ptr->clearButtonEnabled = enabled;
}

bool LineEdit::isClearButtonEnabled() const {
    return d_ptr->clearButtonEnabled;
}

void LineEdit::setFrame(bool enabled) {
    d_ptr->frame = enabled;
    
    if (d_ptr->lvgl_textarea) {
        lv_obj_set_style_border_width(d_ptr->lvgl_textarea, enabled ? 1 : 0, 0);
    }
}

bool LineEdit::hasFrame() const {
    return d_ptr->frame;
}

bool LineEdit::isModified() const {
    return d_ptr->modified;
}

void LineEdit::setModified(bool modified) {
    d_ptr->modified = modified;
}

void LineEdit::setUndoRedoEnabled(bool enabled) {
    d_ptr->undoRedoEnabled = enabled;
}

bool LineEdit::isUndoRedoEnabled() const {
    return d_ptr->undoRedoEnabled;
}

void LineEdit::setContextMenuPolicy(ContextMenuPolicy policy) {
    d_ptr->contextMenuPolicy = policy;
}

ContextMenuPolicy LineEdit::contextMenuPolicy() const {
    return d_ptr->contextMenuPolicy;
}

// 事件处理的空实现
void LineEdit::keyPressEvent(KeyEvent* event) {
    // TODO: 实现按键事件处理
}

void LineEdit::keyReleaseEvent(KeyEvent* event) {
    // TODO: 实现按键释放事件处理
}

void LineEdit::mousePressEvent(MouseEvent* event) {
    // TODO: 实现鼠标按下事件处理
}

void LineEdit::mouseMoveEvent(MouseEvent* event) {
    // TODO: 实现鼠标移动事件处理
}

void LineEdit::mouseReleaseEvent(MouseEvent* event) {
    // TODO: 实现鼠标释放事件处理
}

void LineEdit::focusInEvent(FocusEvent* event) {
    // TODO: 实现获得焦点事件处理
}

void LineEdit::focusOutEvent(FocusEvent* event) {
    editingFinished.emit();
}

void LineEdit::paintEvent(PaintEvent* event) {
    // TODO: 实现绘制事件处理
}

void LineEdit::resizeEvent(ResizeEvent* event) {
    // TODO: 实现大小改变事件处理
}

void LineEdit::contextMenuEvent(ContextMenuEvent* event) {
    // TODO: 实现上下文菜单事件处理
}

void LineEdit::dragEnterEvent(DragEvent* event) {
    // TODO: 实现拖拽进入事件处理
}

void LineEdit::dragMoveEvent(DragEvent* event) {
    // TODO: 实现拖拽移动事件处理
}

void LineEdit::dragLeaveEvent(DragEvent* event) {
    // TODO: 实现拖拽离开事件处理
}

void LineEdit::dropEvent(DropEvent* event) {
    // TODO: 实现放置事件处理
}

// 自动补全方法的空实现
void LineEdit::showCompletions() {
    // TODO: 实现显示自动补全
}

void LineEdit::hideCompletions() {
    // TODO: 实现隐藏自动补全
}

void LineEdit::selectCompletion(int index) {
    // TODO: 实现选择自动补全项
}

int LineEdit::xToPos(int x) const {
    // TODO: 实现坐标到文本位置的转换
    return 0;
}

int LineEdit::posToX(int pos) const {
    // TODO: 实现文本位置到坐标的转换
    return 0;
}

// LVGL回调函数
void LineEdit::onTextChanged(lv_event_t* e) {
    LineEdit* lineEdit = static_cast<LineEdit*>(lv_event_get_user_data(e));
    if (lineEdit && !lineEdit->d_ptr->internalTextChange) {
        const char* text = lv_textarea_get_text(lineEdit->d_ptr->lvgl_textarea);
        lineEdit->d_ptr->text = String(text);
        lineEdit->textChanged.emit(lineEdit->d_ptr->text);
    }
}

void LineEdit::onValueChanged(lv_event_t* e) {
    LineEdit* lineEdit = static_cast<LineEdit*>(lv_event_get_user_data(e));
    if (lineEdit) {
        lineEdit->textEdited.emit(lineEdit->d_ptr->text);
    }
}

void LineEdit::onInsert(lv_event_t* e) {
    LineEdit* lineEdit = static_cast<LineEdit*>(lv_event_get_user_data(e));
    if (lineEdit) {
        // TODO: 处理插入事件
    }
}

void LineEdit::onReady(lv_event_t* e) {
    LineEdit* lineEdit = static_cast<LineEdit*>(lv_event_get_user_data(e));
    if (lineEdit) {
        lineEdit->returnPressed.emit();
        lineEdit->editingFinished.emit();
    }
}

void LineEdit::onCancel(lv_event_t* e) {
    LineEdit* lineEdit = static_cast<LineEdit*>(lv_event_get_user_data(e));
    if (lineEdit) {
        lineEdit->editingFinished.emit();
    }
}

} // namespace QGL