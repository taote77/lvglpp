#ifndef TEMP_CURVE_DLG_H
#define TEMP_CURVE_DLG_H

#include "core/widgets/Dialog.h"
#include "core/widgets/Widget.h"
#include "core/widgets/Image.h"
#include "core/widgets/Chart.h"
#include "core/widgets/Label.h"

#include <memory>
#include <vector>

using namespace lvglpp;
using namespace lvglpp::widgets;

class TempCurveDlg : public widgets::Dialog
{
public:
    explicit TempCurveDlg();

    void addChartIndex(int index);

protected:
    void init();

private:
    void updateTime();

    static void draw_event_cb(lv_event_t *e);

private:
    std::shared_ptr<widgets::Widget> bg_;
    std::shared_ptr<widgets::Image>    back_icon_;
    std::shared_ptr<widgets::Label>   peak_force_curve_label;

    std::shared_ptr<widgets::Label>          abnormal_btn_label_;
    std::shared_ptr<lvglpp::widgets::Chart> chart_;
    std::shared_ptr<widgets::Label>          force_label_;
    std::shared_ptr<widgets::Label>          time_label_;
    std::shared_ptr<widgets::Label>          time_label_1_;
    std::shared_ptr<widgets::Label>          time_label_2_;
    std::shared_ptr<widgets::Label>          time_label_3_;
    std::shared_ptr<widgets::Label>          zero_label_;

    std::shared_ptr<widgets::Label> peak_value_;
    std::shared_ptr<widgets::Label> trough_value_;

    lv_chart_series_t  *ser1_;
    lv_chart_series_t  *ser2_;
    std::vector<double> chart_vec_;
};

#endif // TEMP_CURVE_DLG_H
