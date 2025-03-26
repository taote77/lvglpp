#ifndef _LVPP_BUTTON_H
#define _LVPP_BUTTON_H

#include "core/widgets/BaseItem.h"
#include <string>

namespace heygears {

namespace widgets {

class LvButton : public BaseItem
{
public:
    LvButton(BaseItem *parent = nullptr);

    // LvButton(int width, int height, const std::string &title, BaseItem *parent = nullptr);

    ~LvButton();
};

} // namespace widgets
} // namespace heygears

#endif // _LVPP_BUTTON_H