
#include "ButtonBuilder.h"

namespace lvglpp::widgets {

ButtonBuilder &ButtonBuilder::setWidth(int width)
{
    width_ = width;
    return *this;
}

ButtonBuilder &ButtonBuilder::setHeight(int height)
{
    height_ = height;
    return *this;
}

ButtonBuilder &ButtonBuilder::setColorStyle(PushButton::ColorStyle type)
{
    type_ = type;
    return *this;
}

ButtonBuilder &ButtonBuilder::setText(const std::string &text)
{
    text_ = text;
    return *this;
}

ButtonBuilder &ButtonBuilder::setImagePath(const std::string &path)
{
    image_path_ = path;
    return *this;
}

ButtonBuilder &ButtonBuilder::setRepeatMs(int ms)
{
    repeat_ms_ = ms;
    return *this;
}

ButtonBuilder &ButtonBuilder::setOnClickedListener(const std::function<void()> &cb)
{
    on_click_cb_ = cb;
    return *this;
}

// 创建并返回 PushButton 对象
std::unique_ptr<PushButton> ButtonBuilder::build(BaseItem *parent)
{
    auto button = std::make_unique<PushButton>(width_, height_, type_, text_, image_path_, parent);

    if (repeat_ms_ > 0)
    {
        button->setRepeatMs(repeat_ms_);
    }

    if (on_click_cb_)
    {
        button->setOnClickedListener(on_click_cb_);
    }

    return button;
}

} // namespace lvglpp::widgets