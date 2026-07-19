#ifndef WRAPPER_COL_LAYOUT_H
#define WRAPPER_COL_LAYOUT_H

#include "core/widgets/BaseItem.h" // for BaseItem
#include <memory>                  // for shared_ptr, make_shared
#include <stdint.h>                // for int32_t

namespace wrapper::widgets {
using namespace lvglpp;
using namespace lvglpp::widgets;
class ColLayout : public lvglpp::widgets::BaseItem
{
public:
    ColLayout(uint32_t width, uint32_t height, widgets::BaseItem *parent = nullptr);

    void setPadHor(int lenth);

    void setPadRow(int lenth);

    void setPadBottom(int lenth);

private:
};

}; // namespace wrapper::widgets

#endif // WRAPPER_COL_LAYOUT_H
