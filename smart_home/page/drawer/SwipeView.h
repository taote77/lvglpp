#include "core/widgets/BaseItem.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/ScrollItem.h"
#include <vector>

namespace lvglpp::ui::page {
using namespace lvglpp::widgets;

class SwipeView : public ScrollItem
{
public:
    explicit SwipeView(BaseItem *parent = nullptr);

    ~SwipeView();

    /**
     * @brief
     * 弹性卷轴效果，默认有。 一般情况，对象不能滚动超出其内容的极限
     * @param enable
     */
    void enableBounce(bool enable);

    void addPage(std::shared_ptr<BaseItem> page);

    void enableNavButtons(bool enable = true);

    void SetItemSpace(short space);

    void setCurrentIndex(int n)
    {
        _current_index = n;
    }

    int getCurrentIndex() const
    {
        return _current_index;
    }

private:
    void init();

    void createPages();

    void addLeftButton();

    void addRightButton();

    static void scrollEventHandler(lv_event_t *e);

private:
    int                                    _page_count = 0;
    std::vector<std::shared_ptr<BaseItem>> _pages;

    std::shared_ptr<RoundedButton> _left_btn;
    std::shared_ptr<RoundedButton> _right_btn;

    int _current_index = -1;
};

} // namespace lvglpp::ui::page