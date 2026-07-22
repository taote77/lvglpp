#ifndef LVGLPP_ROLLER_H
#define LVGLPP_ROLLER_H

#include "BaseItem.h"
#include <string>

namespace lvglpp::widgets {

class Roller : public BaseItem
{
public:
    explicit Roller(BaseItem *parent = nullptr);

    /// Set options (separated by \n)
    void setOptions(const std::string &options, lv_roller_mode_t mode = LV_ROLLER_MODE_NORMAL);

    /// Set selected index
    void setSelected(uint32_t index, bool animate = false);

    /// Get selected index
    uint32_t selected() const;

    /// Get the selected option text
    std::string selectedText() const;

    /// Get the number of options
    uint32_t optionCount() const;

    /// Set visible row count
    void setVisibleRowCount(uint32_t rows);
};

} // namespace lvglpp::widgets

#endif // LVGLPP_ROLLER_H
