#include "SwipeView.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/PushButton.h"
#include "theme/theme_keeper.h"

namespace lvglpp::ui::page {

SwipeView::SwipeView(BaseItem *parent) : ScrollItem(parent)
{
    init();
}

SwipeView::~SwipeView()
{}

void SwipeView::enableBounce(bool enable)
{
    if (enable)
    {
        lv_obj_add_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ELASTIC);
    } else
    {
        lv_obj_clear_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ELASTIC);
    }
}

void SwipeView::addPage(std::shared_ptr<BaseItem> page)
{
    page->setParent(this);
    _pages.push_back(page);
}

void SwipeView::enableNavButtons(bool enable)
{
    if (enable)
    {
        addLeftButton();
        addRightButton();
    } else
    {}
}

void SwipeView::SetItemSpace(short space)
{
    lv_obj_set_style_pad_column(this->getLvglItem(), space, LV_STATE_DEFAULT);
}

void SwipeView::init()
{
    setScrollable(true);
    lv_obj_add_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_scroll_snap_x(getLvglItem(), LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(getLvglItem(), LV_FLEX_FLOW_ROW);
    lv_obj_set_scrollbar_mode(getLvglItem(), LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_column(getLvglItem(), 0, LV_STATE_DEFAULT);
    lv_obj_clear_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ELASTIC);

    lv_obj_add_event_cb(
        getLvglItem(),
        [](lv_event_t *e) -> void {
            auto lv_obj   = static_cast<lv_obj_t *>(lv_event_get_target(e));
            auto scroll_x = lv_obj_get_scroll_x(lv_obj);
            // LV_LOG_USER("scroll end!!!!!!x:%d", scroll_x);
            auto data = (SwipeView *)lv_event_get_user_data(e);
            if (data != nullptr)
            {
                int cr_index = scroll_x / data->getWidth();
                if (data->_current_index == cr_index)
                {
                    return;
                }
                // LogDebug<<"scroll end!!!!!!x:"<<scroll_x;
                data->_current_index = cr_index;
                data->createPages();
                // LV_LOG_USER("index changed!!!!!!index:%d", data->current_index_);
                // if (data->current_index_changed_cb_ != nullptr)
                // {
                //     data->current_index_changed_cb_(data->_current_index);
                // }
            }
        },
        LV_EVENT_SCROLL_END, this);

    // peak_force_curve_label =
}

void SwipeView::createPages()
{
    if (_current_index + 2 > 0)
    {
        for (int i = 0; i < _page_count; ++i)
        {
            // TODO
        }
    }
}

void SwipeView::addLeftButton()
{
    _left_btn = std::make_shared<RoundedButton>(240, 64, RoundedButton::Blue, ("<-"), getParent());
    _left_btn->setAligment(LV_ALIGN_TOP_MID, 0, 376);
    _left_btn->setPos(150, 380);
}

void SwipeView::addRightButton()
{
    _right_btn = std::make_shared<RoundedButton>(240, 64, RoundedButton::Blue, ("->"), this->getParent());
    _right_btn->setAligment(LV_ALIGN_TOP_MID, 0, 376);
    _right_btn->setPos(408, 380);
}

void SwipeView::scrollEventHandler(lv_event_t *evt)
{}

} // namespace lvglpp::ui::page
