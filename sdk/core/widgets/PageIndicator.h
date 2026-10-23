#ifndef LVGLPP_PAGEINDICATOR_H
#define LVGLPP_PAGEINDICATOR_H
#include "Widget.h"
#include <memory>
#include <vector>

namespace lvglpp::widgets {

class PageIndicator : public Widget
{
public:
    explicit PageIndicator(int count, Widget *parent = nullptr);
    explicit PageIndicator(Widget *parent = nullptr);
    void changeCurrentIndex(int n);
    void setPageCount(int n);

private:
    int                                    count_;
    int                                    current_index_;
    std::vector<std::unique_ptr<Widget>> indicator_vec_;
    void                                   update();
};
} // namespace lvglpp::widgets

#endif // LVGLPP_PAGEINDICATOR_H
