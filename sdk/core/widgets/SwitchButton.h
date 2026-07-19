#ifndef LVGLPP_SWITCHBUTTON_H
#define LVGLPP_SWITCHBUTTON_H

#include "core/widgets/BaseItem.h"
#include <memory>

namespace lvglpp::widgets {

class SwitchButton : public BaseItem
{
public:
    /**
     * @brief 构造一个开关按钮控件。
     *
     * @param width 按钮的宽度。
     * @param height 按钮的高度。
     * @param parent 父控件指针，默认为 nullptr。
     */
    explicit SwitchButton(lv_coord_t width, lv_coord_t height, BaseItem *parent = nullptr);

    void setCheck(bool checked);

    bool isCheck() const
    {
        return check_state_;
    }

protected:
    void update();

private:
    lv_coord_t                width_{64};
    lv_coord_t                height_{32};
    lv_coord_t                radius_{14};
    bool                      check_state_;
    std::unique_ptr<BaseItem> round_item_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_SWITCHBUTTON_H
