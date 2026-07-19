#ifndef LVGLPP_PAGEINDICATOR_H
#define LVGLPP_PAGEINDICATOR_H
#include "BaseItem.h"
#include <memory>
#include <vector>

namespace lvglpp::widgets {

class PageIndicator : public BaseItem
{
public:
    explicit PageIndicator(int count, BaseItem *parent = nullptr);
    explicit PageIndicator(BaseItem *parent = nullptr);
    void changeCurrentIndex(int n);
    void setPageCount(int n);

private:
    int                                    count_;
    int                                    current_index_;
    std::vector<std::unique_ptr<BaseItem>> indicator_vec_;
    void                                   update();
};
} // namespace lvglpp::widgets

#endif // LVGLPP_PAGEINDICATOR_H
