#ifndef LV_CHECKBOX_H
#define LV_CHECKBOX_H

#include "BaseItem.h"
#include "MouseArea.h"
#include <memory>

namespace lvglpp::widgets {

class CheckBox : public BaseItem
{
public:
    explicit CheckBox(int width, int height, uint32_t bg_clr, uint32_t check_clr, BaseItem *parent = nullptr);

    bool getCheckState();

private:
    std::unique_ptr<BaseItem> check_item_;

    std::unique_ptr<MouseArea> check_mouse_area_;

    void init();

    bool check_state_ = false;
};

} // namespace lvglpp::widgets

#endif // LV_CHECKBOX_H
