#include "Dropdown.h"
#include "core/base/LvObjFactory.h"
#include <sstream>

namespace lvglpp::widgets {

Dropdown::Dropdown(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglDropdown, parent)
{}

void Dropdown::setText(const std::string &text)
{
    lv_dropdown_set_text(getLvglItem(), text.c_str());
}

std::string Dropdown::text() const
{
    const char *t = lv_dropdown_get_text(getLvglItem());
    return t ? std::string(t) : std::string();
}

void Dropdown::setOptions(const std::string &options)
{
    lv_dropdown_set_options(getLvglItem(), options.c_str());
}

void Dropdown::setOptions(const std::vector<std::string> &options)
{
    std::ostringstream oss;
    for (size_t i = 0; i < options.size(); ++i) {
        if (i > 0) oss << "\n";
        oss << options[i];
    }
    setOptions(oss.str());
}

std::string Dropdown::options() const
{
    const char *opts = lv_dropdown_get_options(getLvglItem());
    return opts ? std::string(opts) : std::string();
}

void Dropdown::setSelected(uint32_t index)
{
    lv_dropdown_set_selected(getLvglItem(), index);
}

uint32_t Dropdown::selected() const
{
    return lv_dropdown_get_selected(getLvglItem());
}

uint32_t Dropdown::optionCount() const
{
    return lv_dropdown_get_option_count(getLvglItem());
}

std::string Dropdown::selectedText() const
{
    char buf[256] = {};
    lv_dropdown_get_selected_str(getLvglItem(), buf, sizeof(buf));
    return std::string(buf);
}

void Dropdown::setDirection(lv_dir_t dir)
{
    lv_dropdown_set_dir(getLvglItem(), dir);
}

lv_dir_t Dropdown::direction() const
{
    return lv_dropdown_get_dir(getLvglItem());
}

void Dropdown::setSymbol(const std::string &symbol)
{
    lv_dropdown_set_symbol(getLvglItem(), symbol.c_str());
}

lv_obj_t *Dropdown::listObj() const
{
    return lv_dropdown_get_list(getLvglItem());
}

} // namespace lvglpp::widgets
