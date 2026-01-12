#include "SensorActivity.h"
#include "color_sheet.h"
#include "core/sys/StackView.h"
#include "page/sensor/TempCurveDlg.h"
#include <boost/format.hpp>
#include <random>

using namespace form::design;

SensorActivity::SensorActivity() : BaseActivity()
{}

void SensorActivity::onCreate(void *arg)
{
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    force_dlg_ = new TempCurveDlg(); // std::make_shared<ui::dialogs::TempCurveDlg>();
    force_dlg_->setAutoDel(false);

    btn_ = std::make_shared<widgets::RoundedButton>(200, 64, widgets::RoundedButton::Dark, "开始显示曲线", getRoot());
    btn_->setPos(30, 200);

    btn_->setOnClickedListener([this]() {
        // wait_tips_label_->setText("I am short");

        LOG_DEBUG() << " fddd ";
        force_dlg_->open();
    });

    remain_time_refresh_timer_ = lv_timer_create(
        [](struct _lv_timer_t *tt) -> void {
            auto user_data = lv_timer_get_user_data(tt);
            auto page      = reinterpret_cast<SensorActivity *>(user_data);
            if (page != nullptr)
            {
                static std::random_device                     rd;        // 非确定性随机种子（硬件熵源）
                static std::mt19937                           gen(rd()); // Mersenne Twister 算法引擎
                static std::uniform_real_distribution<double> dist(-100.0, 100.0);
                double                                        random_float = dist(gen);
                random_float *= 100;

                page->force_dlg_->addChartIndex(static_cast<int>(random_float));
            }
        },
        200, this);
}

void SensorActivity::onDestroy()
{
    BaseActivity::onDestroy();
}

void SensorActivity::onNotifyUI(const Event &evt)
{
    BaseActivity::onNotifyUI(evt);
}