#include "TopDrawer.h"
#include "core/widgets/BaseItem.h"
#include "core/kernel/LvObjFactory.h"
#include "core/widgets/PageIndicator.h"
#include "theme/form_design.h"
#include <cmath>

namespace lvglpp::ui::page {
using namespace form::design;

constexpr int content_width{800};
constexpr int content_height{480};

TopDrawer::TopDrawer(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglSysDialog, parent)
{
    setSize(content_width, content_height);
    setOpacity(0);
    setClip(false);
    setBgColor(CLR_SURFACE);
    setEnable(false);

    bg_item_ = std::make_shared<BaseItem>(this);
    bg_item_->setSize(content_width, 450);
    bg_item_->setPos(0, -content_height);
    bg_item_->setOpacity(1);

    _page_indicator = std::make_shared<PageIndicator>(bg_item_.get());
    _page_indicator->setAligment(LV_ALIGN_TOP_MID, 0, 50);
    _page_indicator->setPageCount(2);

    constexpr int container_width  = 672 + 64 * 2;
    constexpr int container_height = 368;

    _page_container = std::make_shared<SwipeView>(bg_item_.get());
    _page_container->setOpacity(1);
    _page_container->setSize(container_width, container_height);
    _page_container->setPos(0, 80);
    _page_container->setPaddingHor(64);

    _page_container->SetItemSpace(64 * 2);

    _mod_panel = std::make_shared<ModuleCollector>(this);
    _page_container->addPage(_mod_panel);

    _notice_panel = std::make_shared<MessageCenterControl>(this);
    _page_container->addPage(_notice_panel);

    touch_area_ = std::make_shared<MouseArea>(this);
    touch_area_->setOpacity(0);
    touch_area_->setSize(300, 60);
    touch_area_->setAligment(LV_ALIGN_TOP_MID, 0, -10);
    touch_area_->setMousePressingListener([this](const Point &pt) -> void { changePagePos(pt.y); });
    touch_area_->setMouseReleasedListener([this](const Point &pt) -> void {
        if (pt.y < 240)
        {
            pagePopAnim();
        } else
        {
            pageEnterAnim();
        }
    });

    _ctrl_panel = std::make_shared<widgets::BaseItem>(this);
    _ctrl_panel->setSize(200, 8);
    _ctrl_panel->setOpacity(1);

    _ctrl_panel->setBgColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER_HIGHEST));

    _ctrl_panel->setRadius(100);
    _ctrl_panel->setAligment(LV_ALIGN_TOP_MID, 0, 6);
    _ctrl_panel->setEnable(false);
}

void TopDrawer::updateDeviceInfo(void *data)
{
    static int filter_step{0};
    filter_step++;

    if (filter_step >= 5)
    {
        _mod_panel->updateDeviceInfo(data);
        filter_step = 0;
    }
}

void TopDrawer::conStateChange(int type, bool on) // type 0-wifi，1-cloud, 2-udisk
{
    if (type == 0)
    {
        _notice_panel->wifiChange(on);
    } else if (type == 1)
    {
        _notice_panel->cloudChange(on);
    } else if (type == 2)
    {
        _notice_panel->udiskChange(on);
    }
}

void TopDrawer::hide()
{
    if (_ctrl_panel)
    {
        _ctrl_panel->setVisible(false);
    }
    if (touch_area_)
    {
        touch_area_->setEnable(false);
    }
}

void TopDrawer::show()
{
    if (_ctrl_panel)
    {
        _ctrl_panel->setVisible(true);
    }
    if (touch_area_)
    {
        touch_area_->setEnable(true);
    }
}

void TopDrawer::changePagePos(int ypos)
{
    int item_y = std::min(ypos - 480, 0);

    bg_item_->setPos(0, (lv_coord_t)item_y);

    touch_area_->setPos(0, ypos - 30);
    int bar_y = -(ypos / 480.0) * 18 + 6;

    _ctrl_panel->setAligmentTo(*touch_area_, LV_ALIGN_CENTER, 0, bar_y);

    //        std::cout << "bar_y: " << bar_y<<" " << ypos<< std::endl;
    static auto stepPercentage = [](double input) -> double {
        input = std::clamp(input, 0.0, 1.0);
        return std::round(input * 10.0) / 10.0;
    };

    static double last_per{0.0};

    auto percent = stepPercentage(ypos / 480.0);
    if (std::abs(last_per - percent) > 0.0001)
    {
        setOpacity(percent);
        last_per = percent;
    }
}

void TopDrawer::pageEnterAnim()
{
    lv_anim_t a_page_enter;
    lv_anim_init(&a_page_enter);
    lv_anim_set_time(&a_page_enter, 300);
    lv_anim_set_var(&a_page_enter, this);
    lv_anim_set_exec_cb(&a_page_enter, [](void *obj, int32_t v) -> void {
        auto p = (TopDrawer *)obj;
        if (p != nullptr)
        {
            p->changePagePos(v);
        }
    });
    lv_anim_set_ready_cb(&a_page_enter, [](struct _lv_anim_t *p) -> void {
        auto d = (TopDrawer *)p->var;
        if (d != nullptr)
        {
            d->setEnable(true);
        }
    });
    lv_anim_set_values(&a_page_enter, bg_item_->getY() + 480, 480);
    lv_anim_start(&a_page_enter);
}

void TopDrawer::pagePopAnim()
{
    lv_anim_t a_page_pop;
    lv_anim_init(&a_page_pop);
    lv_anim_set_time(&a_page_pop, 300);
    lv_anim_set_var(&a_page_pop, this);
    lv_anim_set_exec_cb(&a_page_pop, [](void *obj, int32_t v) -> void {
        auto p = (TopDrawer *)obj;
        if (p != nullptr)
        {
            p->changePagePos(v);
        }
    });
    lv_anim_set_ready_cb(&a_page_pop, [](struct _lv_anim_t *p) -> void {
        auto d = (TopDrawer *)p->var;
        if (d != nullptr)
        {
            d->setEnable(false);
        }
    });
    lv_anim_set_values(&a_page_pop, bg_item_->getY() + 480, 0);
    lv_anim_start(&a_page_pop);
}

} // namespace lvglpp::ui::page
