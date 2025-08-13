#ifndef TEMP_CURVE_DLG_H
#define TEMP_CURVE_DLG_H

#include "core/widgets/BaseDialog.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Image.h"
#include "core/widgets/LvChart.h"
#include "core/widgets/LvText.h"

#include <memory>
#include <vector>

using namespace lvglpp;
using namespace lvglpp::widgets;

class TempCurveDlg : public widgets::BaseDialog
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
    std::shared_ptr<widgets::BaseItem> bg_;
    std::shared_ptr<widgets::Image>    back_icon_;
    std::shared_ptr<widgets::LvText>   peak_force_curve_label;
    // std::shared_ptr<widgets::Image> abnormal_icon_btn_;
    std::shared_ptr<widgets::LvText>          abnormal_btn_label_;
    std::shared_ptr<lvglpp::widgets::LvChart> chart_;
    std::shared_ptr<widgets::LvText>          force_label_;
    std::shared_ptr<widgets::LvText>          time_label_;
    std::shared_ptr<widgets::LvText>          time_label_1_;
    std::shared_ptr<widgets::LvText>          time_label_2_;
    std::shared_ptr<widgets::LvText>          time_label_3_;
    std::shared_ptr<widgets::LvText>          zero_label_;

    std::shared_ptr<widgets::LvText> peak_value_;
    std::shared_ptr<widgets::LvText> trough_value_;

    lv_chart_series_t  *ser1_;
    lv_chart_series_t  *ser2_;
    std::vector<double> chart_vec_;
};

#endif // TEMP_CURVE_DLG_H
