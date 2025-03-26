#ifndef LV_LISTVIEW_H
#define LV_LISTVIEW_H

#include "ScrollItem.h"
#include "ListViewDelegate.hpp"
#include "BaseModel.hpp"
#include <memory>
#include "lvgl/src/misc/lv_timer.h"
// #include "lv_timer.h"
// #include <chrono>

namespace heygears {
namespace widgets {
template <typename T, typename T2 = ListViewDelegate<T>>
class ListView : public ScrollItem
{
public:
    explicit ListView(BaseItem *parentItem = nullptr);

    explicit ListView(const BaseModel<T> &model, BaseItem *parentItem = nullptr);

    ~ListView() override
    {
        removeTimer();
        // show_content_vec_.clear();
        // LV_LOG_USER("ListView delete~~");
    }

    void setModel(const BaseModel<T> &model)
    {
        // list_view_model_.reset(model);
        list_view_model_ = model;
        // update();
    }

    bool changeModelData(int index, const T &data);

    BaseModel<T> *getModel() { return &list_view_model_; }

    void update();

    void setSpan(int n);

    int getSpan() const { return span_; }

    void setDelegateHeight(int n)
    {
        delegateHeight_ = n;
        // update();
    }

    int getDelegateHeight() const { return delegateHeight_; }

    void setAsyncLoad(bool b) { async_load_ = b; }

    bool getAsyncLoad() const { return async_load_; }

    void setExtraData(void *p) { extra_data_ = p; }

    void *getExtraData() const { return extra_data_; }

protected:
    void initItem();

private:
    BaseModel<T> list_view_model_;
    std::vector<std::shared_ptr<T2>> show_content_vec_;
    int span_ = 0;
    int delegateHeight_ = -1;
    int refresh_page_index_ = -1;
    bool async_load_ = false;
    lv_timer_t *async_load_timer_ = nullptr;
    void *extra_data_ = nullptr;

    void removeTimer()
    {
        if (async_load_timer_ != nullptr) {
            lv_timer_del(async_load_timer_);
            async_load_timer_ = nullptr;
        }
    }
};

template <typename T, typename T2>
ListView<T, T2>::ListView(BaseItem *parentItem) : ScrollItem(parentItem)
{
    initItem();
}

template <typename T, typename T2>
ListView<T, T2>::ListView(const BaseModel<T> &model, BaseItem *parentItem)
    : ScrollItem(parentItem), list_view_model_(model)
{
    initItem();
    // update();
    // LV_LOG_USER("ListView create!!%ld", getLvglItem());
}

template <typename T, typename T2>
void ListView<T, T2>::update()
{
    if (delegateHeight_ == -1) {
        show_content_vec_.clear();
        if (async_load_ && list_view_model_.count() > 5) {
            int repeat_times = (list_view_model_.count()) / 5;
            // LV_LOG_USER("async load repeat_times:%d",repeat_times);
            async_load_timer_ = lv_timer_create(
                    [](lv_timer_t *t) -> void {
                        auto user_data = lv_timer_get_user_data(t);
                        auto p = (ListView *)user_data;
                        if (p != nullptr) {
                            int start_index = p->show_content_vec_.size();
                            int end_index =
                                    std::min(start_index + 5, p->list_view_model_.count() - 1);
                            if (end_index >= start_index) {
                                for (int i = start_index; i <= end_index; i++) {
                                    std::shared_ptr<T2> p2(new T2(i, p));
                                    p2->initItem(p->list_view_model_.getItem(i));
                                    p->show_content_vec_.push_back(p2);
                                }
                                // LV_LOG_USER("async load item from %d to
                                // %d",start_index,end_index);
                            }
                        }
                        // if (t->repeat_count == 0)
                        {
                            // p->removeTimer();
                            // LV_LOG_USER("async load timer del");
                        }
                    },
                    100, this);
            lv_timer_set_repeat_count(async_load_timer_, repeat_times);
        } else {
            for (int i = 0; i < list_view_model_.count(); i++) {
                std::shared_ptr<T2> p(new T2(i, this));
                p->initItem(list_view_model_.getItem(i));
                show_content_vec_.push_back(p);
            }
        }
    } else {
        lv_coord_t scroll_y = lv_obj_get_scroll_y(getLvglItem());
        int page_count = scroll_y / getHeight();
        int page_item_count = (getHeight() / (delegateHeight_ + span_)) + 1;
        if (refresh_page_index_ >= page_count) {
            return;
        }
        int prev_page_index = std::max(0, refresh_page_index_);
        refresh_page_index_ = page_count;
        int first_index = 0;
        int last_index = 0;
        if (show_content_vec_.empty()) {
            last_index = std::min((refresh_page_index_ + 2) * page_item_count,
                                  list_view_model_.count() - 1);
            ;
        } else {
            first_index = std::min((prev_page_index + 2) * page_item_count + 1,
                                   list_view_model_.count() - 1);
            last_index = std::min((refresh_page_index_ + 2) * page_item_count,
                                  list_view_model_.count() - 1);
        }
        if (last_index <= first_index) {
            return;
        }
        // show_content_vec_.clear();
        // LV_LOG_USER("refresh item from %d to %d,prev
        // index:%d,index:%d",first_index,last_index,prev_page_index,refresh_page_index_);
        for (int i = first_index; i <= last_index; i++) {
            std::shared_ptr<T2> p(new T2(i, this));
            p->initItem(list_view_model_.getItem(i));
            show_content_vec_.push_back(p);
        }
    }
}

template <typename T, typename T2>
void ListView<T, T2>::initItem()
{
    setScrollable(true);
    lv_obj_set_flex_flow(getLvglItem(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(getLvglItem(), LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_row(getLvglItem(), span_, LV_STATE_DEFAULT);

    lv_obj_add_event_cb(
            getLvglItem(),
            [](lv_event_t *e) -> void {
                auto data = (ListView *)lv_event_get_user_data(e);
                if (data != nullptr) {
                    if (data->getDelegateHeight() != -1) {
                        data->update();
                    }
                }
            },
            LV_EVENT_SCROLL_END, this);
}

template <typename T, typename T2>
void ListView<T, T2>::setSpan(int n)
{
    span_ = n;
    lv_obj_set_style_pad_row(getLvglItem(), span_, LV_STATE_DEFAULT);
}

template <typename T, typename T2>
bool ListView<T, T2>::changeModelData(int index, const T &data)
{
    if (index < 0 || index >= list_view_model_.count()) {
        return false;
    }
    if (!list_view_model_.changed(index, data)) {
        return false;
    }
    if (!show_content_vec_[index]->updateItem(data)) {
        return false;
    }
    return true;
}
} // namespace widgets
} // namespace heygears

#endif // LV_LISTVIEW_H
