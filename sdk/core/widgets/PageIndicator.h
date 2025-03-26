#ifndef LV_PAGE_INDICATOR_H
#define LV_PAGE_INDICATOR_H
#include "BaseItem.h"
#include <vector>
#include <memory>
namespace heygears {
namespace widgets {
class PageIndicator : public BaseItem
{
public:
    explicit PageIndicator(int count, BaseItem *parent = nullptr);
    explicit PageIndicator(BaseItem *parent = nullptr);
    void changeCurrentIndex(int n);
    void setPageCount(int n);

private:
    int count_;
    int current_index_;
    std::vector<std::shared_ptr<BaseItem>> indicator_vec_;
    void update();
};
} // namespace widgets
} // namespace heygears

#endif // LV_PAGE_INDICATOR_H
