#ifndef LV_PAGEVIEW_HPP
#define LV_PAGEVIEW_HPP

#include "ScrollItem.h"
#include "PageViewDelegate.hpp"
#include "BaseModel.hpp"
#include <memory>

namespace heygears {
namespace widgets {
template <typename T, typename T2 = PageViewDelegate<T>>
class PageView : public ScrollItem
{
public:
    explicit PageView(BaseItem *parentItem = nullptr);

    explicit PageView(const BaseModel<T> &model, BaseItem *parentItem = nullptr);

    ~PageView() override
    {
        // show_content_vec_.clear();
        // LV_LOG_USER("PageView delete~~");
    }

    void setModel(const BaseModel<T> &model)
    {
        page_view_model_ = model;
        // update();
    }

    BaseModel<T> *getModel() { return &page_view_model_; }

    // void setDelegate(const std::shared_ptr<BaseDelegate<T>> &delegate);

    void setCurrentIndexChangedListener(const std::function<void(int)> &cb)
    {
        current_index_changed_cb_ = cb;
    }

    void setCurrentIndex(int n);

    int getCurrentIndex() const { return current_index_; }

    void update();

protected:
    void initItem();
    void createDelegates();

private:
    BaseModel<T> page_view_model_;
    std::function<void(int)> current_index_changed_cb_ = nullptr;
    int current_index_ = -1;
    std::vector<std::shared_ptr<T2>> show_content_vec_;
    // int cache_delegate_size = 5;
};

template <typename T, typename T2>
PageView<T, T2>::PageView(BaseItem *parentItem) : ScrollItem(parentItem), show_content_vec_(50)
{
    initItem();
}

template <typename T, typename T2>
PageView<T, T2>::PageView(const BaseModel<T> &model, BaseItem *parentItem)
    : ScrollItem(parentItem), page_view_model_(model), show_content_vec_(50)
{
    initItem();
}

template <typename T, typename T2>
void PageView<T, T2>::initItem()
{
    setScrollable(true);
    lv_obj_add_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_scroll_snap_x(getLvglItem(), LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(getLvglItem(), LV_FLEX_FLOW_ROW);
    lv_obj_set_scrollbar_mode(getLvglItem(), LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_column(getLvglItem(), 0, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(
            getLvglItem(),
            [](lv_event_t *e) -> void {
                auto lobj = static_cast<lv_obj_t *>(lv_event_get_target(e));
                // auto scroll_x = lv_obj_get_scroll_x(lv_event_get_target(e));
                auto scroll_x = lv_obj_get_scroll_x(lobj);
                // LV_LOG_USER("scroll end!!!!!!x:%d", scroll_x);
                auto data = (PageView *)lv_event_get_user_data(e);
                if (data != nullptr) {
                    int cr_index = scroll_x / data->getWidth();
                    if (data->current_index_ == cr_index) {
                        return;
                    }
                    // LogDebug<<"scroll end!!!!!!x:"<<scroll_x;
                    data->current_index_ = cr_index;
                    data->createDelegates();
                    // LV_LOG_USER("index changed!!!!!!index:%d", data->current_index_);
                    if (data->current_index_changed_cb_ != nullptr) {
                        data->current_index_changed_cb_(data->current_index_);
                    }
                }
            },
            LV_EVENT_SCROLL_END, this);
}

template <typename T, typename T2>
void PageView<T, T2>::update()
{
    show_content_vec_.clear();
    /*for (int i = 0; i < page_view_model_.count(); i++)
    {
        std::shared_ptr<T2> p(new T2(this));
        p->initItem(page_view_model_.getItem(i));
        show_content_vec_.push_back(p);
    }
    current_index_ = page_view_model_.count() > 0 ? 0 : -1;*/
    if (page_view_model_.count() > 0) {
        current_index_ = 0;
        createDelegates();
    } else {
        current_index_ = -1;
    }
}

template <typename T, typename T2>
void PageView<T, T2>::setCurrentIndex(int n)
{
    int scroll_x = n * getWidth();
    lv_obj_scroll_to_x(getLvglItem(), scroll_x, LV_ANIM_ON);
}

template <typename T, typename T2>
void PageView<T, T2>::createDelegates()
{
    /*while(current_index_+2>show_content_vec_.size())
    {
        if(show_content_vec_.size()>=page_view_model_.count())
            break;
        int index=show_content_vec_.size();
        std::shared_ptr<T2> p(new T2(this));
        p->initItem(page_view_model_.getItem(index));
        show_content_vec_.push_back(p);
        //LogDebug<<"create index:"<<index<<",current size:"<<show_content_vec_.size();
    }*/
    if (current_index_ + 2 > show_content_vec_.size()
        && show_content_vec_.size() < page_view_model_.count()) {
        int index = show_content_vec_.size();
        std::shared_ptr<T2> p(new T2(this));
        p->initItem(page_view_model_.getItem(index));
        show_content_vec_.push_back(p);
        // LogDebug<<"create index:"<<index<<",current size:"<<show_content_vec_.size();

        if (current_index_ + 2 > show_content_vec_.size()) {
            lv_async_call(
                    [](void *src) -> void {
                        auto p = (PageView *)src;
                        if (p != nullptr) {
                            p->createDelegates();
                        }
                    },
                    this);
        }
    }
}
} // namespace widgets
} // namespace heygears

#endif // LV_PAGEVIEW_HPP
