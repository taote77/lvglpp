#include "TopDrawer.h"
#include "core/sys/UiConfig.h"
#include "core/widgets/Widget.h"
#include "core/base/LvObjFactory.h"
#include "core/widgets/PageIndicator.h"
#include "theme/form_design.h"
#include <algorithm>
#include <cmath>

namespace lvglpp::ui::page {
using namespace form::design;

TopDrawer::TopDrawer(Widget *parent) : Widget(tools::LvObjFactory::createLvglSysDialog, parent)
{
    const auto &cfg       = sys::UiConfig::instance();
    const int   width     = cfg.width();
    content_height_       = cfg.height();

    setSize(width, content_height_);
    setOpacity(0);
    setClip(false);
    setBgColor(CLR_SURFACE);
    setEnabled(false);

    bg_item_ = std::make_shared<Widget>(this);
    bg_item_->setSize(width, content_height_ - 30);
    bg_item_->setPos(0, -content_height_);
    bg_item_->setOpacity(1);

    _page_indicator = std::make_shared<PageIndicator>(bg_item_.get());
    _page_indicator->setAlignment(LV_ALIGN_TOP_MID, 0, 50);
    _page_indicator->setPageCount(2);

    const int container_width  = std::min(width, 672 + 64 * 2);
    const int container_height = std::min(content_height_ - 112, 368);

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
    touch_area_->setAlignment(LV_ALIGN_TOP_MID, 0, -10);
    touch_area_->setMousePressingListener([this](const Point &pt) -> void { changePagePos(pt.y); });
    touch_area_->setMouseReleasedListener([this](const Point &pt) -> void {
        if (pt.y < content_height_ / 2)
        {
            pagePopAnim();
        } else
        {
            pageEnterAnim();
        }
    });

    _ctrl_panel = std::make_shared<widgets::Widget>(this);
    _ctrl_panel->setSize(200, 8);
    _ctrl_panel->setOpacity(1);

    _ctrl_panel->setBgColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER_HIGHEST));

    _ctrl_panel->setRadius(100);
    _ctrl_panel->setAlignment(LV_ALIGN_TOP_MID, 0, 6);
    _ctrl_panel->setEnabled(false);
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
        touch_area_->setEnabled(false);
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
        touch_area_->setEnabled(true);
    }
}

void TopDrawer::changePagePos(int ypos)
{
    int item_y = std::min(ypos - content_height_, 0);

    bg_item_->setPos(0, (lv_coord_t)item_y);

    touch_area_->setPos(0, ypos - 30);
    int bar_y = -(ypos / static_cast<double>(content_height_)) * 18 + 6;

    _ctrl_panel->setAlignmentTo(*touch_area_, LV_ALIGN_CENTER, 0, bar_y);

    //        std::cout << "bar_y: " << bar_y<<" " << ypos<< std::endl;
    static auto stepPercentage = [](double input) -> double {
        input = std::clamp(input, 0.0, 1.0);
        return std::round(input * 10.0) / 10.0;
    };

    static double last_per{0.0};

    auto percent = stepPercentage(ypos / static_cast<double>(content_height_));
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
            d->setEnabled(true);
        }
    });
    lv_anim_set_values(&a_page_enter, bg_item_->getY() + content_height_, content_height_);
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
            d->setEnabled(false);
        }
    });
    lv_anim_set_values(&a_page_pop, bg_item_->getY() + content_height_, 0);
    lv_anim_start(&a_page_pop);
}

} // namespace lvglpp::ui::page
