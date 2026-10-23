/**************************************************************************
Description:圆角按钮

**************************************************************************/

#ifndef LVGLPP_PUSHBUTTON_H
#define LVGLPP_PUSHBUTTON_H

#include "Image.h"
#include "Label.h"
#include "Widget.h"
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>

namespace lvglpp::widgets {

class PushButton : public Widget
{
public:
    enum ColorStyle {
        Blue,
        Dark,
        Gray,
        LightBlue,
        BlackBroder,
        Red,
        DarkGray,
    };
    PushButton(int width, int height, ColorStyle type, const std::string &strMsg, Widget *parent);

    PushButton(int width, int height, ColorStyle type, const std::string &strMsg, const std::string &strImgPath, Widget *parent);

    void setImagePos(int x, int y);

    void setImagePos(lv_align_t align, int x, int y);

    void setTextPos(int x, int y);

    void setTextPos(lv_align_t align, int x, int y);

    void setTextAlignment(lv_text_align_t align_type);

    void setText(const std::string &str);

    [[deprecated("use setText()")]]
    void setTextMsg(const std::string &str)
    {
        setText(str);
    }

    void setEnabled(bool enable) override;

    void setImageAsset(const std::string &assetPath);

    [[deprecated("use setImageAsset()")]]
    void changeImageAsset(const std::string &assetPath)
    {
        setImageAsset(assetPath);
    }

    void setOnClickedListener(const std::function<void()> &l) override;

    void setColorStyle(ColorStyle type);

    [[deprecated("use setColorStyle()")]]
    void setColorType(ColorStyle type)
    {
        setColorStyle(type);
    }

    void setRepeatMs(int n)
    {
        repeat_ms_ = n;
    }

private:
    std::unique_ptr<Label>                label_;
    std::unique_ptr<widgets::Image>       img_obj_;
    PushButton::ColorStyle                type_;
    int                                   repeat_ms_ = 500;
    std::chrono::system_clock::time_point prev_time_point_;
    std::function<void()>                 onButtonClicked_cb_ = nullptr;

    void init(int width, int height, ColorStyle type);

    void initLabel(const std::string &msg, PushButton::ColorStyle type);

    void initImage(const std::string &url);

    void handleButtonClicked();
};

} // namespace lvglpp::widgets

#endif // LVGLPP_PUSHBUTTON_H
