#ifndef FORCE_CHART_ACTIVITY_H
#define FORCE_CHART_ACTIVITY_H

#include "TempCurveDlg.h"
#include "core/sys/BaseActivity.h"
#include "core/widgets/GifImage.h"
#include "core/widgets/Image.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include <memory>

using namespace lvglpp;
using namespace lvglpp::sys;

class SensorActivity : public BaseActivity
{
public:
    explicit SensorActivity();

protected:
    void onCreate(std::any arg) override;

    void onNotifyUI(const Event &evt) override;

    void onDestroy() override;

private:
    std::shared_ptr<widgets::LvText> wait_tips_label_;

    std::shared_ptr<widgets::GifImage> gif_icon_;
    std::shared_ptr<widgets::LvText>   wait_tips_label_2_;

    std::shared_ptr<widgets::Image> img_;

    std::shared_ptr<widgets::RoundedButton> btn_;

    TempCurveDlg *force_dlg_;

    lv_timer_t *remain_time_refresh_timer_{nullptr};
};

#endif // FORCE_CHART_ACTIVITY_H