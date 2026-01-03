#ifndef LV_BUTTON_BUILDER_H
#define LV_BUTTON_BUILDER_H

#include "PushButton.h" // 确保包含 PushButton 的头文件
#include <memory>
#include <string>

namespace lvglpp::widgets {

class ButtonBuilder
{
public:
    ButtonBuilder() = default;

    ButtonBuilder &setWidth(int width);

    ButtonBuilder &setHeight(int height);

    ButtonBuilder &setColorStyle(PushButton::ColorStyle type);

    ButtonBuilder &setText(const std::string &text);

    ButtonBuilder &setImagePath(const std::string &path);

    ButtonBuilder &setRepeatMs(int ms);

    ButtonBuilder &setOnClickedListener(const std::function<void()> &cb);

    // 创建并返回 PushButton 对象
    std::unique_ptr<PushButton> build(BaseItem *parent);

private:
    int                    width_  = 100;
    int                    height_ = 40;
    PushButton::ColorStyle type_   = PushButton::Blue;
    std::string            text_;
    std::string            image_path_;
    int                    repeat_ms_ = 500;
    std::function<void()>  on_click_cb_;
};

} // namespace lvglpp::widgets

#endif // LV_BUTTON_BUILDER_H