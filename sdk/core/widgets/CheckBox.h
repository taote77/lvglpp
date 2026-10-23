#ifndef LVGLPP_CHECKBOX_H
#define LVGLPP_CHECKBOX_H

#include "Widget.h"
#include "MouseArea.h"
#include <memory>

namespace lvglpp::widgets {

class CheckBox : public Widget
{
public:
    explicit CheckBox(int width, int height, uint32_t bg_clr, uint32_t check_clr, Widget *parent = nullptr);

    bool getCheckState();

private:
    std::unique_ptr<Widget> check_item_;

    std::unique_ptr<MouseArea> check_mouse_area_;

    void init();

    bool check_state_ = false;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_CHECKBOX_H
