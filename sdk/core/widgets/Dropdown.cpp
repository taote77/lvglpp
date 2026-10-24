#include "Dropdown.h"
#include "core/base/LvObjFactory.h"
#include "core/sys/UiConfig.h"
#include <sstream>

namespace lvglpp::widgets {

namespace {

// LVGL always anchors the option list to OUT_BOTTOM_LEFT/OUT_TOP_LEFT. In RTL
// the visual anchor should be the right edge; realign after the list opens.
void alignListForDirection(lv_event_t *event)
{
#if LVGLPP_ENABLE_RTL
    if (!sys::UiConfig::instance().isRtl())
    {
        return;
    }

    auto *dropdown = static_cast<lv_obj_t *>(lv_event_get_target(event));
    if (dropdown == nullptr)
    {
        return;
    }

    lv_obj_t *list = lv_dropdown_get_list(dropdown);
    if (list == nullptr)
    {
        return;
    }

    const lv_dir_t dir    = lv_dropdown_get_dir(dropdown);
    const lv_align_t align = (dir == LV_DIR_TOP) ? LV_ALIGN_OUT_TOP_RIGHT : LV_ALIGN_OUT_BOTTOM_RIGHT;
    lv_obj_align_to(list, dropdown, align, 0, 0);
#else
    (void)event;
#endif
}

} // namespace

Dropdown::Dropdown(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglDropdown, parent)
{
    setFocusable(true);
    addLvglEventHandler(alignListForDirection, LV_EVENT_CLICKED, nullptr);
    addLvglEventHandler(alignListForDirection, LV_EVENT_VALUE_CHANGED, nullptr);
}

void Dropdown::setText(const std::string &text)
{
    lv_dropdown_set_text(lvObj(), text.c_str());
}

std::string Dropdown::text() const
{
    const char *t = lv_dropdown_get_text(lvObj());
    return t ? std::string(t) : std::string();
}

void Dropdown::setOptions(const std::string &options)
{
    lv_dropdown_set_options(lvObj(), options.c_str());
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
    const char *opts = lv_dropdown_get_options(lvObj());
    return opts ? std::string(opts) : std::string();
}

void Dropdown::setSelected(uint32_t index)
{
    lv_dropdown_set_selected(lvObj(), index);
}

uint32_t Dropdown::selected() const
{
    return lv_dropdown_get_selected(lvObj());
}

uint32_t Dropdown::optionCount() const
{
    return lv_dropdown_get_option_count(lvObj());
}

std::string Dropdown::selectedText() const
{
    char buf[256] = {};
    lv_dropdown_get_selected_str(lvObj(), buf, sizeof(buf));
    return std::string(buf);
}

Connection Dropdown::onChanged(std::function<void(uint32_t)> handler)
{
    return onEvent(LV_EVENT_VALUE_CHANGED, [this, handler = std::move(handler)]() {
        if (handler)
        {
            handler(selected());
        }
    });
}

void Dropdown::setDirection(lv_dir_t dir)
{
    lv_dropdown_set_dir(lvObj(), dir);
}

lv_dir_t Dropdown::direction() const
{
    return lv_dropdown_get_dir(lvObj());
}

void Dropdown::setSymbol(const std::string &symbol)
{
    lv_dropdown_set_symbol(lvObj(), symbol.c_str());
}

lv_obj_t *Dropdown::listObj() const
{
    return lv_dropdown_get_list(lvObj());
}

} // namespace lvglpp::widgets
