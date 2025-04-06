#include "BaseItem.h"
#include "core/tools/LvglUtils.h"

namespace lvglpp::widgets {

BaseItem::BaseItem(BaseItem *parentItem) : BaseItem(BaseItem::NormalItem, parentItem)
{}

BaseItem::BaseItem(BaseItem::ItemType type, BaseItem *parentItem) : parent(parentItem)
{
    initItem(type);
}

BaseItem::~BaseItem()
{
    if (getParent() == nullptr)
    {
        if (lv_base_ptr_ != nullptr)
        {
            // LV_LOG_USER("remove!!%ld",lv_base_ptr_);
            lv_obj_del(lv_base_ptr_);
            lv_base_ptr_ = nullptr;
        }
    }
}

void BaseItem::createElement(ItemType type)
{
    lv_obj_t *parent_lvgl_obj;
    if (parent == nullptr)
    {
        // lv_base_ptr_=tools::LvglUtils::createLvglItem(lv_scr_act());
        parent_lvgl_obj = lv_scr_act();
    } else
    {
        // lv_base_ptr_=tools::LvglUtils::createLvglItem(parent->getLvglItem());
        parent_lvgl_obj = parent->getLvglItem();
    }
    switch (type)
    {
    case ItemType::NormalItem:
        lv_base_ptr_ = tools::LvglUtils::createLvglItem(parent_lvgl_obj);
        break;
    case ItemType::Image:
        lv_base_ptr_ = tools::LvglUtils::createLvglImage(parent_lvgl_obj);
        break;
    case ItemType::Text:
        lv_base_ptr_ = tools::LvglUtils::createLvglLabel(parent_lvgl_obj);
        break;
    case ItemType::SpinBox:
        lv_base_ptr_ = tools::LvglUtils::createLvglSpinBox(parent_lvgl_obj);
        break;
    case ItemType::GIF:
        lv_base_ptr_ = tools::LvglUtils::createLvglGif(parent_lvgl_obj);
        break;
    case ItemType::Dialog:
        lv_base_ptr_ = tools::LvglUtils::createLvglDialog();
        break;
    case ItemType::Progress:
        lv_base_ptr_ = tools::LvglUtils::createLvglProgress(parent_lvgl_obj);
        break;
    case ItemType::SysDialog:
        lv_base_ptr_ = tools::LvglUtils::createLvglSysDialog();
        break;
    case ItemType::Chart:
        lv_base_ptr_ = tools::LvglUtils::createLvglChart(parent_lvgl_obj);
        break;
    case ItemType::Video:
        lv_base_ptr_ = tools::LvglUtils::createLvglVideo(parent_lvgl_obj);
        break;
    default:
        break;
    }
}

void BaseItem::registerEvent()
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_add_event_cb(
            lv_base_ptr_,
            [](lv_event_t *evt) -> void {
                auto user_data = lv_event_get_user_data(evt);
                if (user_data != nullptr)
                {
                    auto item_data = (BaseItem *)user_data;
                    if (lv_event_get_code(evt) == LV_EVENT_CLICKED)
                    {
                        if (item_data->clicked_cb_ != nullptr)
                        {
                            item_data->clicked_cb_();
                        }
                    }
                }
            },
            LV_EVENT_CLICKED, this);

        lv_obj_add_event_cb(
            lv_base_ptr_,
            [](lv_event_t *evt) -> void {
                auto user_data = lv_event_get_user_data(evt);

                if (user_data != nullptr)
                {
                    auto item_data = (BaseItem *)user_data;
                    if (lv_event_get_code(evt) == LV_EVENT_PRESSED)
                    {
                        if (item_data->pressed_cb_ != nullptr)
                        {
                            item_data->pressed_cb_();
                        }
                    }
                }
            },
            LV_EVENT_PRESSED, this);

        lv_obj_add_event_cb(
            lv_base_ptr_,
            [](lv_event_t *evt) -> void {
                auto user_data = lv_event_get_user_data(evt);

                if (user_data != nullptr)
                {
                    auto item_data = (BaseItem *)user_data;
                    if (lv_event_get_code(evt) == LV_EVENT_RELEASED)
                    {
                        if (item_data->released_cb_ != nullptr)
                        {
                            item_data->released_cb_();
                        }
                    }
                }
            },
            LV_EVENT_RELEASED, this);
    }
}

void BaseItem::initItem(ItemType type)
{
    createElement(type);
    registerEvent();
}

void BaseItem::setVisible(bool visible)
{
    if (visible)
    {
        lv_obj_clear_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
    } else
    {
        lv_obj_add_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
    }
}

bool BaseItem::getVisible() const
{
    return !lv_obj_has_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
}

void BaseItem::setEnable(bool enable)
{
    if (enable)
    {
        lv_obj_add_flag(lv_base_ptr_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_state(lv_base_ptr_, LV_STATE_DISABLED);
    } else
    {
        lv_obj_clear_flag(lv_base_ptr_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(lv_base_ptr_, LV_STATE_DISABLED);
    }
}

bool BaseItem::getEnable() const
{
    return lv_obj_has_flag(lv_base_ptr_, LV_OBJ_FLAG_CLICKABLE);
}

void BaseItem::setPos(lv_coord_t x, lv_coord_t y)
{
    lv_obj_set_pos(lv_base_ptr_, x, y);
}

lv_coord_t BaseItem::getY() const
{
    return lv_obj_get_y(lv_base_ptr_);
}

void BaseItem::setAligment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
    lv_obj_align(lv_base_ptr_, align, offsetX, offsetY);
}

void BaseItem::setAligmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
    lv_obj_align_to(lv_base_ptr_, item.getLvglItem(), align, offsetX, offsetY);
}

void BaseItem::setBgColor(uint32_t clr)
{
    lv_obj_set_style_bg_color(lv_base_ptr_, lv_color_hex(clr), LV_STATE_DEFAULT);
}

void BaseItem::setDisableBgColor(uint32_t clr)
{
    lv_obj_set_style_bg_color(lv_base_ptr_, lv_color_hex(clr), LV_STATE_DISABLED);
}

void BaseItem::setOpacity(double opa)
{
    int lvgl_opa_value = (int)(LV_OPA_COVER * opa);
    lv_obj_set_style_opa(lv_base_ptr_, lvgl_opa_value, LV_STATE_DEFAULT);
}

void BaseItem::setSize(lv_coord_t width, lv_coord_t height)
{
    width_  = width;
    height_ = height;
    lv_obj_set_size(lv_base_ptr_, width, height);
}

void BaseItem::setWidth(lv_coord_t w)
{
    width_ = w;
    lv_obj_set_width(lv_base_ptr_, width_);
}

void BaseItem::setRadius(lv_coord_t radius)
{
    lv_obj_set_style_radius(lv_base_ptr_, radius, LV_STATE_DEFAULT);
}

void BaseItem::setClip(bool clip)
{
    lv_obj_set_style_clip_corner(lv_base_ptr_, clip, LV_STATE_DEFAULT);
}

lv_coord_t BaseItem::getRadius() const
{
    return lv_obj_get_style_radius(lv_base_ptr_, LV_STATE_DEFAULT);
}

void BaseItem::setBorder(lv_coord_t width, uint32_t color) const
{
    lv_obj_set_style_border_width(getLvglItem(), width, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(getLvglItem(), lv_color_hex(color), LV_STATE_DEFAULT);
}

lv_coord_t BaseItem::getWidth() const
{
    return width_;
}

lv_coord_t BaseItem::getHeight() const
{
    return height_;
}

void BaseItem::setParent(BaseItem *parent)
{
    lv_obj_set_parent(this->getLvglItem(), parent->getLvglItem());
}

void BaseItem::setPaddingBottom(lv_coord_t padding)
{
    lv_obj_set_style_pad_bottom(lv_base_ptr_, padding, LV_STATE_DEFAULT);
}

void BaseItem::setPaddingTop(lv_coord_t padding)
{
    lv_obj_set_style_pad_top(lv_base_ptr_, padding, LV_STATE_DEFAULT);
}

void BaseItem::setPaddingHor(lv_coord_t padding)
{
    lv_obj_set_style_pad_hor(lv_base_ptr_, padding, LV_STATE_DEFAULT);
}

} // namespace lvglpp::widgets
