#include "Drawer.h"
#include "core/widgets/color_sheet.h"

namespace lvglpp::ui::page {

using namespace form::design;

constexpr int content_width{800};
constexpr int content_height{480};

Drawer::Drawer(Drawer::Edge edge, BaseItem *parent) : BaseItem(BaseItem::ItemType::SysDialog, parent), _edge(edge)
{
    setSize(content_width, content_height);
    setOpacity(0);
    setClip(false);
    setBgColor(CLR_SURFACE);
    setEnable(false);

    _root_container = std::make_shared<BaseItem>(this);
    _root_container->setSize(content_width, content_height);
    _root_container->setOpacity(1);
    _root_container->setBgColor(CLR_SURFACE_CONTAINER_LOWEST);

    touch_area_ = std::make_shared<MouseArea>(this);
    touch_area_->setOpacity(0);

    if (_edge == Edge::TopEdge)
    {
        _root_container->setPos(0, -content_height + 20);

        touch_area_->setSize(400, 60);
        touch_area_->setAligment(LV_ALIGN_TOP_MID, 0, -10);
        touch_area_->setMousePressingListener([this](const Point &pt) -> void {
            //
            changePagePos(pt.y);
        });
        touch_area_->setMouseReleasedListener([this](const Point &pt) -> void {
            if (pt.y < content_height / 2)
            {
                pagePopAnim();
            } else
            {
                pageEnterAnim();
            }
        });
    } else
    {
        _root_container->setPos(-content_width + 20, 0);

        touch_area_->setSize(60, 300);
        touch_area_->setAligment(LV_ALIGN_LEFT_MID, 0, -10);
        touch_area_->setMousePressingListener([this](const Point &pt) -> void {
            //
            changePagePos(pt.x);
        });
        touch_area_->setMouseReleasedListener([this](const Point &pt) -> void {
            if (pt.x < content_width / 2)
            {
                pagePopAnim();
            } else
            {
                pageEnterAnim();
            }
        });
    }
}

void Drawer::changePagePos(int pos)
{
    if (_edge == Edge::TopEdge)
    {
        int item_y = std::min(pos - content_height, 0);
        _root_container->setPos(0, (lv_coord_t)item_y);
        touch_area_->setPos(0, pos - 30);
    } else
    {
        int item_x = std::min(pos - content_width, 0);
        _root_container->setPos((lv_coord_t)item_x, 0);
        touch_area_->setPos(pos - 30, 0);
    }
}

void Drawer::pageEnterAnim()
{
    lv_anim_t a_page_enter;
    lv_anim_init(&a_page_enter);
    lv_anim_set_time(&a_page_enter, 300);
    lv_anim_set_var(&a_page_enter, this);
    lv_anim_set_exec_cb(&a_page_enter, [](void *obj, int32_t v) -> void {
        auto page = (Drawer *)obj;
        if (page != nullptr)
        {
            page->changePagePos(v);
        }
    });
    lv_anim_set_ready_cb(&a_page_enter, [](struct _lv_anim_t *p) -> void {
        auto page = (Drawer *)p->var;
        if (page != nullptr)
        {
            page->setEnable(true);
        }
    });
    lv_anim_set_completed_cb(&a_page_enter, [](struct _lv_anim_t *p) -> void {
        auto page = (Drawer *)p->var;
        if (page != nullptr)
        {
            page->setOpacity(1);
        }
    });

    if (_edge == Edge::TopEdge)
    {
        lv_anim_set_values(&a_page_enter, _root_container->getY() + content_height, content_height);
    } else
    {
        lv_anim_set_values(&a_page_enter, _root_container->getX() + content_width, content_width);
    }

    lv_anim_start(&a_page_enter);
}

void Drawer::pagePopAnim()
{
    lv_anim_t a_page_pop;
    lv_anim_init(&a_page_pop);
    lv_anim_set_time(&a_page_pop, 300);
    lv_anim_set_var(&a_page_pop, this);
    lv_anim_set_exec_cb(&a_page_pop, [](void *obj, int32_t v) -> void {
        auto page = (Drawer *)obj;
        if (page != nullptr)
        {
            page->changePagePos(v);
        }
    });
    lv_anim_set_ready_cb(&a_page_pop, [](struct _lv_anim_t *p) -> void {
        auto page = (Drawer *)p->var;
        if (page != nullptr)
        {
            page->setEnable(false);
        }
    });

    lv_anim_set_completed_cb(&a_page_pop, [](struct _lv_anim_t *p) -> void {
        auto page = (Drawer *)p->var;
        if (page != nullptr)
        {
            page->setOpacity(0);
        }
    });

    if (_edge == Edge::TopEdge)
    {
        lv_anim_set_values(&a_page_pop, _root_container->getY() + content_height, 20);
    } else
    {
        lv_anim_set_values(&a_page_pop, _root_container->getX() + content_width, 20);
    }

    lv_anim_start(&a_page_pop);
}

} // namespace lvglpp::ui::page