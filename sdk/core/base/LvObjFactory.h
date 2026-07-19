#ifndef LVOBJ_FACTORY_H
#define LVOBJ_FACTORY_H

#include "lvgl.h"

/*******************************************************************************
 * @brief lvgl对象工厂类
 * @note Factory Pattern, 用于创建各种控件
 * @details
 ******************************************************************************/

namespace lvglpp::tools {

class LvObjFactory
{
public:
    LvObjFactory() = delete;

    LvObjFactory(const LvObjFactory &) = delete;

    LvObjFactory operator=(const LvObjFactory &) = delete;

    /**
     * 创建一个不可滑动的lvgl对象
     * @param parent
     * @return
     */
    static lv_obj_t *createLvglItem(lv_obj_t *parent);

    /**
     * 创建一个可滑动的lvgl对象
     * @param parent
     * @return
     */
    static lv_obj_t *createLvglScrolable(lv_obj_t *parent);

    /**
     * 创建一个lvgl image对象
     * @param parent
     * @return
     */
    static lv_obj_t *createLvglImage(lv_obj_t *parent);

    static lv_obj_t *createLvglLabel(lv_obj_t *parent);

    static lv_obj_t *createLvglSpinBox(lv_obj_t *parent);

    static lv_obj_t *createLvglGif(lv_obj_t *parent);

    static lv_obj_t *createLvglDialog(lv_obj_t *parent = nullptr);

    static lv_obj_t *createLvglSysDialog(lv_obj_t *parent = nullptr);

    static lv_obj_t *createLvglProgress(lv_obj_t *parent);

    static lv_obj_t *createLvglChart(lv_obj_t *parent);

    static lv_obj_t *createLvglVideo(lv_obj_t *parent);

    static lv_obj_t *createLvglLottie(lv_obj_t *parent);

    static lv_obj_t *createLvglQrCode(lv_obj_t *parent);
};
// Modern alias
using WidgetFactory = LvObjFactory;

} // namespace lvglpp::tools

#endif // LVOBJ_FACTORY_H
