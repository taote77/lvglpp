#ifndef LVGLPP_SWITCH_H
#define LVGLPP_SWITCH_H

#include "core/widgets/Widget.h"
#include <memory>

namespace lvglpp::widgets {

class Switch : public Widget
{
public:
    /**
     * @brief 构造一个开关按钮控件。
     *
     * @param width 按钮的宽度。
     * @param height 按钮的高度。
     * @param parent 父控件指针，默认为 nullptr。
     */
    explicit Switch(lv_coord_t width, lv_coord_t height, Widget *parent = nullptr);

    void setChecked(bool checked);

    bool isChecked() const
    {
        return check_state_;
    }

protected:
    void update();

private:
    lv_coord_t              width_{64};
    lv_coord_t              height_{32};
    lv_coord_t              radius_{14};
    bool                    check_state_{false};
    std::unique_ptr<Widget> round_item_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_SWITCH_H
