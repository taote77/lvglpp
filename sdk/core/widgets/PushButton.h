/**************************************************************************
Description:圆角按钮

**************************************************************************/

#ifndef LVGLPP_PUSHBUTTON_H
#define LVGLPP_PUSHBUTTON_H

#include "BaseItem.h"
#include "Image.h"
#include "LvText.h"
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>

namespace lvglpp::widgets {

class PushButton : public BaseItem
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
    PushButton(int width, int height, ColorStyle type, const std::string &strMsg, BaseItem *parent);

    PushButton(int width, int height, ColorStyle type, const std::string &strMsg, const std::string &strImgPath, BaseItem *parent);

    void setImagePos(int x, int y);

    void setImagePos(lv_align_t align, int x, int y);

    void setTextPos(int x, int y);

    void setTextPos(lv_align_t align, int x, int y);

    void setTextAlignment(lv_text_align_t align_type);

    void setTextMsg(const std::string &str);

    void setEnable(bool enable) override;

    void changeImageUrl(const std::string &url);

    void setOnClickedListener(const std::function<void()> &l) override;

    void setColorType(ColorStyle type);

    void setRepeatMs(int n)
    {
        repeat_ms_ = n;
    }

private:
    std::unique_ptr<LvText>               label_;
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
// Modern aliases
using Button = PushButton;
using RoundedButton = PushButton; // backward compatibility

} // namespace lvglpp::widgets

#endif // LVGLPP_PUSHBUTTON_H
