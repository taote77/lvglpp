/**************************************************************************
Description:圆角按钮

**************************************************************************/

#ifndef LV_ROUNDED_BUTTON_H
#define LV_ROUNDED_BUTTON_H

#include "LvText.h"
#include "Image.h"
#include "BaseItem.h"
#include <memory>
#include <functional>
#include <chrono>
#include <cmath>

namespace lvglpp {
namespace widgets {
class RoundedButton : public BaseItem
{
public:
    enum ColorStyle { Blue, Dark, Gray, LightBlue, BlackBroder, Red };

    RoundedButton(int width, int height, ColorStyle type, const std::string &strMsg,
                  BaseItem *parent);

    RoundedButton(int width, int height, ColorStyle type, const std::string &strMsg,
                  const std::string &strImgPath, BaseItem *parent);

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

    void setRepeatMs(int n) { repeat_ms_ = n; }

private:
    std::shared_ptr<LvText> label_;
    std::shared_ptr<widgets::Image> img_obj_;
    RoundedButton::ColorStyle type_;
    int repeat_ms_ = 500;
    std::chrono::system_clock::time_point prev_time_point_;
    std::function<void()> onButtonClicked_cb_ = nullptr;

    void init(int width, int height, ColorStyle type);

    void initLabel(const std::string &msg, RoundedButton::ColorStyle type);

    void initImage(const std::string &url);

    void handleButtonClicked();
};
} // namespace widgets
} // namespace lvglpp

#endif // LV_ROUNDED_BUTTON_H
