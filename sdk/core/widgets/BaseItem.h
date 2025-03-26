/**************************************************************************

Class:BaseItem

Description:c++ lvgl封装item基类

**************************************************************************/

#ifndef LV_BASE_ITEM_H
#define LV_BASE_ITEM_H

#include "lvgl.h"
#include <functional>

namespace lvglpp::widgets {
class BaseItem
{
public:
    explicit BaseItem(BaseItem *parentItem = nullptr);

    virtual ~BaseItem();

    virtual void setPos(lv_coord_t x, lv_coord_t y);

    lv_coord_t getY() const;

    virtual void setAligment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY);

    virtual void setAligmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX,
                               lv_coord_t offsetY);

    void setBgColor(uint32_t clr);

    void setDisableBgColor(uint32_t clr);

    /**
     * 透明度设置 0.0~1.0
     * @param opa
     */
    void setOpacity(double opa);

    void setSize(lv_coord_t width, lv_coord_t height);

    void setWidth(lv_coord_t w);

    lv_coord_t getWidth() const;

    lv_coord_t getHeight() const;

    virtual void setVisible(bool visible);

    bool getVisible() const;

    virtual void setEnable(bool enable);

    bool getEnable() const;

    void setRadius(lv_coord_t radius);

    lv_coord_t getRadius() const;

    void setClip(bool clip);

    /**
     * 回弹效果
     * @param b
     */

    void setBorder(lv_coord_t width, uint32_t color) const;

    BaseItem *getParent() const { return this->parent; }

    void setParent(BaseItem *parent);

    virtual void setOnClickedListener(const std::function<void()> &l) { this->clicked_cb_ = l; }

    virtual void setOnPressedListener(const std::function<void()> &l) { this->pressed_cb_ = l; }

    virtual void setOnReleasedListener(const std::function<void()> &l) { this->released_cb_ = l; }

    lv_obj_t *getLvglItem() const { return lv_base_ptr_; }

    void setPaddingBottom(lv_coord_t padding);

    void setPaddingTop(lv_coord_t padding);

    void setPaddingHor(lv_coord_t padding);

protected:
    void setLvglItem(lv_obj_t *ptr) { lv_base_ptr_ = ptr; }

    enum ItemType {
        NormalItem,
        Image,
        Text,
        SpinBox,
        GIF,
        Dialog,
        Progress,
        SysDialog,
        Chart,
        Video,
    };

    explicit BaseItem(ItemType type, BaseItem *parentItem = nullptr);

private:
    BaseItem *parent = nullptr;
    lv_obj_t *lv_base_ptr_ = nullptr;
    std::function<void()> clicked_cb_ = nullptr;
    std::function<void()> pressed_cb_ = nullptr;
    std::function<void()> released_cb_ = nullptr;
    lv_coord_t width_ = 0;
    lv_coord_t height_ = 0;

    void initItem(ItemType type);

    /**
     * 生成控件
     */
    void createElement(ItemType type);

    /**
     * 注册监听事件
     */
    void registerEvent();

    friend class BaseDialog;
};
} // namespace lvglpp::widgets

#endif // LV_BASE_ITEM_H
