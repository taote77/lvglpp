#ifndef LVGLPP_TIMER_H
#define LVGLPP_TIMER_H

#include "core/widgets/BaseItem.h"

namespace lvglpp::sys {

using namespace lvglpp::widgets;

class Timer
{
public:
    explicit Timer(BaseItem *parent = nullptr);
    ~Timer();

    void start();
};

} // namespace lvglpp::sys

#endif // LVGLPP_TIMER_H