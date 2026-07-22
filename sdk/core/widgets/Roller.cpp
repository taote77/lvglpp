#include "Roller.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Roller::Roller(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglRoller, parent)
{}

void Roller::setOptions(const std::string &options, lv_roller_mode_t mode)
{
    lv_roller_set_options(getLvglItem(), options.c_str(), mode);
}

void Roller::setSelected(uint32_t index, bool animate)
{
    lv_roller_set_selected(getLvglItem(), index, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

uint32_t Roller::selected() const
{
    return lv_roller_get_selected(getLvglItem());
}

std::string Roller::selectedText() const
{
    char buf[256] = {};
    lv_roller_get_selected_str(getLvglItem(), buf, sizeof(buf));
    return std::string(buf);
}

uint32_t Roller::optionCount() const
{
    return lv_roller_get_option_count(getLvglItem());
}

void Roller::setVisibleRowCount(uint32_t rows)
{
    lv_roller_set_visible_row_count(getLvglItem(), rows);
}

} // namespace lvglpp::widgets
