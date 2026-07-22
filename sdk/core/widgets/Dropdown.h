#ifndef LVGLPP_DROPDOWN_H
#define LVGLPP_DROPDOWN_H

#include "BaseItem.h"
#include <string>
#include <vector>

namespace lvglpp::widgets {

class Dropdown : public BaseItem
{
public:
    explicit Dropdown(BaseItem *parent = nullptr);

    /// Set the currently displayed text
    void setText(const std::string &text);

    /// Get the currently displayed text
    std::string text() const;

    /// Set options: a string separated by \n, e.g. "Option1\nOption2\nOption3"
    void setOptions(const std::string &options);

    /// Set options from a vector
    void setOptions(const std::vector<std::string> &options);

    /// Get the options string
    std::string options() const;

    /// Set the selected option index (0-based)
    void setSelected(uint32_t index);

    /// Get the selected option index
    uint32_t selected() const;

    /// Get the number of options
    uint32_t optionCount() const;

    /// Get the text of the selected option
    std::string selectedText() const;

    /// Set the dropdown open direction (LV_DIR_BOTTOM, LV_DIR_TOP, LV_DIR_LEFT, LV_DIR_RIGHT)
    void setDirection(lv_dir_t dir);

    /// Get the dropdown direction
    lv_dir_t direction() const;

    /// Set the dropdown symbol (defaults to LV_SYMBOL_DOWN)
    void setSymbol(const std::string &symbol);

    /// Get the dropdown list object (advanced use)
    lv_obj_t *listObj() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_DROPDOWN_H
