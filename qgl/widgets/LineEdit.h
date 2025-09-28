#pragma once

#include "Widget.h"
#include "../core/Types.h"
#include "../signals/Signal.h"
#include "../properties/Property.h"
#include <functional>
#include <regex>
#include <climits>
#include <cfloat>

namespace QGL {

// 文本对齐枚举
enum class TextAlignment {
    Left,
    Center,
    Right,
    Justify
};

// 上下文菜单策略
enum class ContextMenuPolicy {
    NoContextMenu,
    DefaultContextMenu,
    ActionsContextMenu,
    CustomContextMenu,
    PreventContextMenu
};

/**
 * @brief 输入验证器类型
 */
enum class ValidatorType {
    None,           // 无验证
    Integer,        // 整数验证
    Double,         // 浮点数验证
    Email,          // 邮箱验证
    Phone,          // 电话号码验证
    Custom          // 自定义验证
};

/**
 * @brief 回显模式
 */
enum class EchoMode {
    Normal,         // 正常显示
    NoEcho,         // 不显示
    Password,       // 密码模式（显示*）
    PasswordEchoOnEdit  // 编辑时显示，其他时候显示*
};

/**
 * @brief 输入验证器基类
 */
class Validator {
public:
    virtual ~Validator() = default;
    virtual bool validate(const String& text) const = 0;
    virtual String fixup(const String& text) const { return text; }
    virtual bool isAcceptableInput(const String& text) const { return validate(text); }
};

/**
 * @brief 整数验证器
 */
class IntValidator : public Validator {
private:
    int m_bottom;
    int m_top;

public:
    IntValidator(int bottom = -2147483648, int top = 2147483647);
    
    bool validate(const String& text) const override;
    String fixup(const String& text) const override;
    
    void setRange(int bottom, int top);
    int bottom() const { return m_bottom; }
    int top() const { return m_top; }
};

/**
 * @brief 双精度浮点验证器
 */
class DoubleValidator : public Validator {
private:
    double m_bottom;
    double m_top;
    int m_decimals;

public:
    DoubleValidator(double bottom = -1.7976931348623158e+308, double top = 1.7976931348623158e+308, int decimals = 1000);
    
    bool validate(const String& text) const override;
    String fixup(const String& text) const override;
    
    void setRange(double bottom, double top, int decimals = 1000);
    double bottom() const { return m_bottom; }
    double top() const { return m_top; }
    int decimals() const { return m_decimals; }
};

/**
 * @brief 正则表达式验证器
 */
class RegExpValidator : public Validator {
private:
    std::regex m_regExp;
    String m_pattern;

public:
    explicit RegExpValidator(const String& pattern);
    
    bool validate(const String& text) const override;
    
    void setRegExp(const String& pattern);
    String regExp() const { return m_pattern; }
};

/**
 * @brief 单行文本输入控件
 * 
 * LineEdit提供了单行文本输入功能，支持文本编辑、验证、格式化、
 * 输入掩码、密码模式等丰富的功能。
 */
class LineEdit : public Widget {
    QGL_OBJECT(LineEdit)

public:
    /**
     * @brief 构造函数
     * @param parent 父控件
     */
    explicit LineEdit(Widget* parent = nullptr);
    
    /**
     * @brief 构造函数
     * @param text 初始文本
     * @param parent 父控件
     */
    explicit LineEdit(const String& text, Widget* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~LineEdit() override;

    // 文本内容
    void setText(const String& text);
    String text() const;
    String displayText() const;  // 考虑回显模式的显示文本
    
    // 占位符文本
    void setPlaceholderText(const String& text);
    String placeholderText() const;
    
    // 选中文本
    void selectAll();
    void deselect();
    bool hasSelectedText() const;
    String selectedText() const;
    int selectionStart() const;
    int selectionLength() const;
    void setSelection(int start, int length);
    
    // 光标位置
    void setCursorPosition(int position);
    int cursorPosition() const;
    void cursorForward(bool mark = false, int steps = 1);
    void cursorBackward(bool mark = false, int steps = 1);
    void cursorWordForward(bool mark = false);
    void cursorWordBackward(bool mark = false);
    void home(bool mark = false);
    void end(bool mark = false);
    
    // 编辑操作
    void clear();
    void copy() const;
    void cut();
    void paste();
    void undo();
    void redo();
    void insert(const String& text);
    void del();
    void backspace();
    
    // 只读模式
    void setReadOnly(bool readOnly);
    bool isReadOnly() const;
    
    // 回显模式
    void setEchoMode(EchoMode mode);
    EchoMode echoMode() const;
    
    // 输入掩码
    void setInputMask(const String& mask);
    String inputMask() const;
    
    // 最大长度
    void setMaxLength(int length);
    int maxLength() const;
    
    // 验证器
    void setValidator(std::shared_ptr<Validator> validator);
    std::shared_ptr<Validator> validator() const;
    bool hasAcceptableInput() const;
    
    // 自动补全
    void setCompleter(const std::vector<String>& completions);
    std::vector<String> completer() const;
    void clearCompleter();
    
    // 拖拽支持
    void setDragEnabled(bool enabled);
    bool isDragEnabled() const;
    
    // 文本格式
    void setAlignment(TextAlignment alignment);
    TextAlignment alignment() const;
    
    // 边距
    void setTextMargins(int left, int top, int right, int bottom);
    void setTextMargins(const Rect& margins);
    Rect textMargins() const;
    
    // 焦点策略
    void setFocusPolicy(FocusPolicy policy);
    FocusPolicy focusPolicy() const;
    
    // 样式相关
    void setClearButtonEnabled(bool enabled);
    bool isClearButtonEnabled() const;
    
    void setFrame(bool enabled);
    bool hasFrame() const;
    
    // 文本修改标志
    bool isModified() const;
    void setModified(bool modified);
    
    // 撤销/重做
    void setUndoRedoEnabled(bool enabled);
    bool isUndoRedoEnabled() const;
    
    // 上下文菜单
    void setContextMenuPolicy(ContextMenuPolicy policy);
    ContextMenuPolicy contextMenuPolicy() const;

    // 信号
    SIGNAL(textChanged, const String&);          // 文本改变
    SIGNAL(textEdited, const String&);           // 文本被编辑
    SIGNAL(editingFinished);                     // 编辑完成
    SIGNAL(returnPressed);                       // 回车键按下
    SIGNAL(selectionChanged);                    // 选择改变
    SIGNAL(cursorPositionChanged, int, int);     // 光标位置改变
    SIGNAL(inputRejected);                       // 输入被拒绝

    // Widget接口重写
    void setEnabled(bool enabled);
    void setVisible(bool visible);
    void setFocus();
    void clearFocus();
    bool hasFocus() const;

protected:
    // 事件处理
    void keyPressEvent(KeyEvent* event) override;
    void keyReleaseEvent(KeyEvent* event) override;
    void mousePressEvent(MouseEvent* event) override;
    void mouseMoveEvent(MouseEvent* event) override;
    void mouseReleaseEvent(MouseEvent* event) override;
    void focusInEvent(FocusEvent* event) override;
    void focusOutEvent(FocusEvent* event) override;
    void paintEvent(PaintEvent* event) override;
    void resizeEvent(ResizeEvent* event) override;
    void contextMenuEvent(ContextMenuEvent* event) override;
    void dragEnterEvent(DragEvent* event) override;
    void dragMoveEvent(DragEvent* event) override;
    void dragLeaveEvent(DragEvent* event) override;
    void dropEvent(DropEvent* event) override;
    
    // 验证
    virtual bool validateInput(const String& text) const;
    virtual String fixupInput(const String& text) const;
    
    // 内部方法
    void updateDisplay();
    void updateCursor();
    void updateSelection();
    String maskString(const String& text) const;
    int xToPos(int x) const;
    int posToX(int pos) const;
    
    // 自动补全
    void showCompletions();
    void hideCompletions();
    void selectCompletion(int index);

private:
    class LineEditPrivate;
    std::unique_ptr<LineEditPrivate> d_ptr;
    
    void initializeLineEdit();
    void setupLVGLTextArea();
    void connectLVGLSignals();
    
    // LVGL回调函数
    static void onTextChanged(lv_event_t* e);
    static void onValueChanged(lv_event_t* e);
    static void onInsert(lv_event_t* e);
    static void onReady(lv_event_t* e);
    static void onCancel(lv_event_t* e);
    
    // 属性声明
    QGL_PROPERTY(String, text, TEXT)
    QGL_PROPERTY(String, placeholderText, PLACEHOLDER_TEXT)
    QGL_PROPERTY(bool, readOnly, READ_ONLY)
    QGL_PROPERTY(EchoMode, echoMode, ECHO_MODE)
    QGL_PROPERTY(String, inputMask, INPUT_MASK)
    QGL_PROPERTY(int, maxLength, MAX_LENGTH)
    QGL_PROPERTY(TextAlignment, alignment, ALIGNMENT)
    QGL_PROPERTY(bool, clearButtonEnabled, CLEAR_BUTTON_ENABLED)
    QGL_PROPERTY(bool, frame, FRAME)
    QGL_PROPERTY(bool, modified, MODIFIED)
    QGL_PROPERTY(bool, undoRedoEnabled, UNDO_REDO_ENABLED)
    QGL_PROPERTY(FocusPolicy, focusPolicy, FOCUS_POLICY)
    QGL_PROPERTY(ContextMenuPolicy, contextMenuPolicy, CONTEXT_MENU_POLICY)
    QGL_PROPERTY(bool, dragEnabled, DRAG_ENABLED)
};

} // namespace QGL