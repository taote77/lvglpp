#include "BaseItem.h"
#include "core/kernel/LvObjFactory.h"
#include <iostream>

namespace lvglpp::widgets {

BaseItem::BaseItem(BaseItem *parentItem)
    : BaseItem(tools::LvObjFactory::createLvglItem, parentItem)
{}

BaseItem::BaseItem(Creator creator, BaseItem *parentItem)
    : parent(parentItem)
{
    initItem(creator);
}

BaseItem::~BaseItem()
{
    if (getParent() == nullptr)
    {
        if (lv_base_ptr_ != nullptr)
        {
            lv_obj_del(lv_base_ptr_);
            lv_base_ptr_ = nullptr;
        }
    }
}

void BaseItem::initItem(const Creator &creator)
{
    lv_obj_t *parent_lvgl_obj = (parent == nullptr) ? lv_scr_act() : parent->getLvglItem();
    lv_base_ptr_ = creator(parent_lvgl_obj);
    registerEvent();
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

lv_coord_t BaseItem::getX() const
{
    return lv_obj_get_x(lv_base_ptr_);
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

void BaseItem::setBorder(lv_coord_t width, uint32_t color)
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

// ---- Introspection / Accessibility API ----

uint32_t BaseItem::childCount() const
{
    return lv_obj_get_child_cnt(lv_base_ptr_);
}

BaseItem *BaseItem::childAt(uint32_t index) const
{
    auto *lv_child = lv_obj_get_child(lv_base_ptr_, index);
    if (!lv_child) return nullptr;
    // Walk through all BaseItems to find one wrapping this lv_obj
    // Note: This is a limitation — we don't store a lv_obj → BaseItem mapping.
    // For the AI agent use case, children are typically created through BaseItem(parent)
    // and we can use lv_obj_get_user_data if set.
    return static_cast<BaseItem *>(lv_obj_get_user_data(lv_child));
}

BaseItem *BaseItem::findChild(const std::string &n) const
{
    if (_name == n) return const_cast<BaseItem *>(this);
    for (uint32_t i = 0; i < childCount(); i++) {
        auto *child = childAt(i);
        if (!child) continue;
        if (auto *found = child->findChild(n))
            return found;
    }
    return nullptr;
}

BaseItem::Rect BaseItem::bounds() const
{
    lv_area_t area;
    lv_obj_get_coords(lv_base_ptr_, &area);
    return {area.x1, area.y1, lv_area_get_width(&area), lv_area_get_height(&area)};
}

std::string BaseItem::describeState() const
{
    std::ostringstream os;
    auto r = bounds();
    os << "{"
       << R"("name":")" << _name << "\","
       << R"("type":")" << typeid(*this).name() << "\","
       << R"("visible":)" << (getVisible() ? "true" : "false") << ","
       << R"("enabled":)" << (getEnable() ? "true" : "false") << ","
       << R"("rect":{"x":)" << r.x << ",\"y\":" << r.y
       << ",\"w\":" << r.w << ",\"h\":" << r.h << "},"
       << R"("children":)" << childCount()
       << "}";
    return os.str();
}

void BaseItem::dumpTree(int depth) const
{
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << (_name.empty() ? "(unnamed)" : _name) << " [" << typeid(*this).name() << "]"
              << " visible=" << getVisible() << " pos=(" << getX() << "," << getY() << ")"
              << " size=(" << getWidth() << "," << getHeight() << ")" << std::endl;
    for (uint32_t i = 0; i < childCount(); i++) {
        if (auto *child = childAt(i))
            child->dumpTree(depth + 1);
    }
}

} // namespace lvglpp::widgets
