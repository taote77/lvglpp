
#include "PageIndicator.h"
#include "config/color_sheet.h"
#include <memory>

namespace lvglpp::widgets {

PageIndicator::PageIndicator(int count, Widget *parent) : Widget(parent), count_(count), current_index_(-1)
{
    setSize((lv_coord_t)(12 * count + 8 * (count - 1)), 12);
    lv_obj_set_flex_flow(lvObj(), LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(lvObj(), 8, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lvObj(), LV_OPA_TRANSP, LV_STATE_DEFAULT);
    for (int i = 0; i < count_; i++)
    {
        indicator_vec_.emplace_back(std::make_unique<Widget>(this));
        indicator_vec_[i]->setSize(12, 12);
        indicator_vec_[i]->setRadius(6);
    }
    update();
}

PageIndicator::PageIndicator(Widget *parent) : Widget(parent), count_(0), current_index_(-1)
{
    lv_obj_set_flex_flow(lvObj(), LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(lvObj(), 8, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lvObj(), LV_OPA_TRANSP, LV_STATE_DEFAULT);
}

void PageIndicator::update()
{
    for (int i = 0; i < count_; i++)
    {
        if (current_index_ == i)
        {
            indicator_vec_[i]->setBgColor(form::design::CLR_SURFACE_CONTAINER_HIGH);
        } else
        {
            indicator_vec_[i]->setBgColor(form::design::CLR_SURFACE_DIS_CONTAINER_1);
        }
    }
}

void PageIndicator::changeCurrentIndex(int n)
{
    if (n == current_index_)
    {
        return;
    }
    current_index_ = n;
    update();
}

void PageIndicator::setPageCount(int n)
{
    count_ = n;
    setSize((lv_coord_t)(12 * count_ + 8 * (count_ - 1)), 12);
    lv_obj_clean(lvObj());
    indicator_vec_.clear();
    for (int i = 0; i < count_; i++)
    {
        indicator_vec_.emplace_back(std::make_unique<Widget>(this));
        indicator_vec_[i]->setSize(12, 12);
        indicator_vec_[i]->setRadius(6);
    }
    update();
}
} // namespace lvglpp::widgets