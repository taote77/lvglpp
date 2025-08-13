#include "TempCurveDlg.h"
#include "core/log/log.h"
#include "core/tools/Utils.h"
#include "core/widgets/color_sheet.h"
#include "i18n/LvTranslator.h"
#include <boost/format.hpp>
#include <memory>
#include <utility>

constexpr int peak_pos_y   = 92;
constexpr int trough_pos_y = 395;

using namespace form::design;

TempCurveDlg::TempCurveDlg() : BaseDialog()
{
    init();
}

void TempCurveDlg::init()
{
    bg_ = std::make_shared<widgets::BaseItem>(this->getRootItem());
    bg_->setSize(800, 480);
    bg_->setBgColor(CLR_SURFACE_DIM);
    bg_->setPos(0, 0);
    bg_->setOpacity(1);

    zero_label_ = std::make_shared<widgets::LvText>("0", CLR_ON_SURFACE, bg_.get());
    zero_label_->setPos(90, 200);

    chart_ = std::make_shared<lvglpp::widgets::LvChart>(bg_.get());
    chart_->setSize(600, 300);
    chart_->setPos(100, 116);

    lv_obj_set_style_pad_all(chart_->getLvglItem(), 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(chart_->getLvglItem(), 2, LV_PART_MAIN);
    lv_obj_set_style_pad_right(chart_->getLvglItem(), 1, LV_PART_MAIN);
    lv_chart_set_type(chart_->getLvglItem(), LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart_->getLvglItem(), LV_CHART_AXIS_PRIMARY_Y, -20, 30);
    lv_chart_set_point_count(chart_->getLvglItem(), 150);
    lv_obj_set_style_size(chart_->getLvglItem(), 0, LV_STATE_DEFAULT, LV_PART_INDICATOR);
    lv_chart_set_div_line_count(chart_->getLvglItem(), 6, 0);
    lv_chart_set_div_line_count(chart_->getLvglItem(), 6, 3);
    lv_obj_set_style_bg_opa(chart_->getLvglItem(), LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(chart_->getLvglItem(), 0, LV_PART_MAIN);

    lv_obj_add_event_cb(chart_->getLvglItem(), TempCurveDlg::draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, this);
    lv_obj_add_flag(chart_->getLvglItem(), LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    time_label_1_ = std::make_shared<LvText>("", CLR_ON_SURFACE, bg_.get());
    lv_obj_align_to(time_label_1_->getLvglItem(), chart_->getLvglItem(), LV_ALIGN_OUT_BOTTOM_MID, 300, 16);

    time_label_2_ = std::make_shared<LvText>("", CLR_ON_SURFACE, bg_.get());
    lv_obj_align_to(time_label_2_->getLvglItem(), chart_->getLvglItem(), LV_ALIGN_OUT_BOTTOM_MID, 0, 16);

    time_label_3_ = std::make_shared<LvText>("", CLR_ON_SURFACE, bg_.get());
    lv_obj_align_to(time_label_3_->getLvglItem(), chart_->getLvglItem(), LV_ALIGN_OUT_BOTTOM_MID, -300, 16);

    ser2_ = lv_chart_add_series(chart_->getLvglItem(), lv_color_hex(CLR_PRIMARY_DARK_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_all_value(chart_->getLvglItem(), ser2_, 0);

    ser1_ = lv_chart_add_series(chart_->getLvglItem(), lv_color_hex(CLR_PRIMARY_BLUE), LV_CHART_AXIS_PRIMARY_Y);

    back_icon_ = std::make_shared<widgets::Image>(":/image/56x56/Back.png", bg_.get());
    back_icon_->setPos(24, 24);
    back_icon_->setEnable(true);
    back_icon_->setOnClickedListener([this]() -> void { this->close(); });

    peak_force_curve_label = std::make_shared<LvText>("", CLR_ON_SURFACE, bg_.get(), LvText::FontSize24);
    peak_force_curve_label->setAligmentTo(*back_icon_, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
    peak_force_curve_label->setReColorEnable(true);

    peak_value_ = std::make_shared<LvText>("0.00 N", CLR_ON_SURFACE, bg_.get(), LvText::FontSize20);
    peak_value_->setPos(28, peak_pos_y);
    trough_value_ = std::make_shared<LvText>("0.00 N", CLR_ON_SURFACE, bg_.get(), LvText::FontSize20);
    trough_value_->setPos(28, trough_pos_y);
}

void TempCurveDlg::updateTime()
{
    time_t time_;
    time(&time_);
    std::string time_text_1 = tools::Utils::convertTimeStampToTimeStr(time_, "%H:%M:%S");
    std::string time_text_2 = tools::Utils::convertTimeStampToTimeStr(time_ - 15, "%H:%M:%S");
    std::string time_text_3 = tools::Utils::convertTimeStampToTimeStr(time_ - 30, "%H:%M:%S");

    time_label_1_->setText(time_text_1);
    time_label_2_->setText(time_text_2);
    time_label_3_->setText(time_text_3);

    lv_obj_align_to(time_label_1_->getLvglItem(), chart_->getLvglItem(), LV_ALIGN_OUT_BOTTOM_MID, 300, 16);
    lv_obj_align_to(time_label_2_->getLvglItem(), chart_->getLvglItem(), LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    lv_obj_align_to(time_label_3_->getLvglItem(), chart_->getLvglItem(), LV_ALIGN_OUT_BOTTOM_MID, -300, 16);
}

static void hook_division_lines(lv_event_t *e) // 内置曲线
{
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    auto            base_dsc  = reinterpret_cast<lv_draw_dsc_base_t *>(lv_draw_task_get_draw_dsc(draw_task));
    auto            line_dsc  = reinterpret_cast<lv_draw_line_dsc_t *>(lv_draw_task_get_draw_dsc(draw_task));

    /*Vertical line*/
    if (line_dsc->p1.x == line_dsc->p2.x)
    {
        //
        line_dsc->width = 0;
        line_dsc->color = lv_color_hex(CLR_SURFACE);

        if (base_dsc->id1 == 1)
        {
            line_dsc->width      = 4;
            line_dsc->dash_gap   = 5;
            line_dsc->dash_width = 5;
            line_dsc->color      = lv_color_hex(CLR_WARNING);
        }

    }
    /*Horizontal line*/
    else
    {
        // if (base_dsc->id1 == 2)
        line_dsc->width      = 1;
        line_dsc->dash_gap   = 5;
        line_dsc->dash_width = 5;
        line_dsc->color      = lv_color_hex(CLR_SURFACE);
    }
}

static void hook_division_lines2(lv_event_t *e) // 自己添加的曲线
{
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    auto            base_dsc  = reinterpret_cast<lv_draw_dsc_base_t *>(lv_draw_task_get_draw_dsc(draw_task));
    auto            line_dsc  = reinterpret_cast<lv_draw_line_dsc_t *>(lv_draw_task_get_draw_dsc(draw_task));

    if (base_dsc->id1 == 1)
    {
        line_dsc->width = 3;
        line_dsc->color = lv_color_hex(CLR_PRIMARY);

    } else
    {
        line_dsc->width = 1;
        line_dsc->color = lv_palette_lighten(LV_PALETTE_GREEN, 1);
    }
}

void TempCurveDlg::draw_event_cb(lv_event_t *e)
{
    auto this_page = (TempCurveDlg *)lv_event_get_user_data(e);
    if (this_page == nullptr)
    {
        return;
    }

    auto draw_task = (lv_draw_task_t *)lv_event_get_draw_task(e);

    auto base_dsc = (lv_draw_dsc_base_t *)(lv_draw_task_get_draw_dsc(draw_task));

    if (base_dsc->part == LV_PART_ITEMS
        // || base_dsc->part == LV_PART_CUSTOM_FIRST
    )
    {
        if (lv_draw_task_get_type(draw_task) == LV_DRAW_TASK_TYPE_LINE)
        {
            hook_division_lines2(e);
        }
    }

    if ((base_dsc->part == LV_PART_MAIN) && lv_draw_task_get_type(draw_task) == LV_DRAW_TASK_TYPE_LINE)
    {
        // LOG_DEBUG() << "=================================";
        hook_division_lines(e);
    }
}

void TempCurveDlg::addChartIndex(int index)
{
    updateTime();

    if (chart_vec_.size() < 150)
    {
        chart_vec_.push_back(index / 100.0);
    } else
    {
        chart_vec_.erase(chart_vec_.begin());
        chart_vec_.push_back(index / 100.0);
    }

    double max_index = *std::max_element(chart_vec_.begin(), chart_vec_.end());
    double min_index = *std::min_element(chart_vec_.begin(), chart_vec_.end());

    peak_force_curve_label->setText((boost::format("Value     Max   %1%      Min  %2%") % max_index % min_index).str());

    zero_label_->setPos(85, peak_pos_y + 10 + (max_index / (max_index - min_index) * (trough_pos_y - peak_pos_y)));

    lv_chart_set_range(chart_->getLvglItem(), LV_CHART_AXIS_PRIMARY_Y, min_index < -10 ? min_index - 10 : -20, max_index > 15 ? max_index + 15 : 30);

    double axis_y_low  = min_index < -10 ? min_index - 10 : -20;
    double axis_y_high = max_index > 15 ? max_index + 15 : 30;

    peak_value_->setText((boost::format("%1% N") % axis_y_low).str());
    trough_value_->setText((boost::format("%1% N") % max_index).str());

    lv_chart_set_next_value(chart_->getLvglItem(), ser1_, index / 100.0);
    lv_chart_set_next_value(chart_->getLvglItem(), ser2_, 0);

    lv_chart_refresh(chart_->getLvglItem());
}
