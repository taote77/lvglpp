#ifndef LVGLPP_MSGBOX_H
#define LVGLPP_MSGBOX_H

#include "Widget.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace lvglpp::widgets {

/**
 * Thin wrapper around lv_msgbox. Footer buttons get Widget wrappers so they
 * participate in focus/keyboard navigation and their callbacks are removed
 * safely with the MsgBox.
 */
class MsgBox : public Widget
{
public:
    explicit MsgBox(Widget *parent = nullptr);

    MsgBox &setTitle(const std::string &title);

    MsgBox &addText(const std::string &text);

    /**
     * Add a footer button. closeOnClick closes the message box after the
     * callback returns.
     */
    Widget *addButton(const std::string &text, std::function<void()> onClick = {}, bool closeOnClick = true);

    void close();

private:
    std::vector<std::unique_ptr<Widget>> button_wrappers_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_MSGBOX_H
