#ifndef WRAPPER_PROGRESS_BAR_H
#define WRAPPER_PROGRESS_BAR_H

#include "core/widgets/BaseItem.h" // for BaseItem
#include <memory>                  // for shared_ptr, make_shared
#include <stdint.h>                // for int32_t

namespace wrapper::widgets {
using namespace lvglpp;
using namespace lvglpp::widgets;

class ProgressBar : public widgets::BaseItem
{
public:
    ProgressBar(widgets::BaseItem *parent, int w, int h, int min = 0, int max = 100, int val = 0);

    static std::shared_ptr<ProgressBar> create(widgets::BaseItem *parent, int w, int h, int min = 0, int max = 100, int val = 0)
    {
        std::shared_ptr<ProgressBar> item = std::make_shared<ProgressBar>(parent, w, h, min, max, val);
        item->setSize(w, h);

        return item;
    }

    void updateValue(int val);

    void setBgRadius(int32_t radius);

    void setIndicatorRadius(int32_t radius);

    void setPadSpace(int32_t sapce);

    void setMainColor(int32_t clr);

    void setIndicatorColor(int32_t clr);

private:
    int _max_value{100};
    int _min_value{0};

    double _value{0.0};
};

}; // namespace wrapper::widgets

#endif // WRAPPER_PROGRESS_BAR_H
