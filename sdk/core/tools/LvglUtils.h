#ifndef LVPP_UTILS_H
#define LVPP_UTILS_H

#include "lvgl.h"

namespace lvglpp {
namespace tools {
class LvglUtils
{
public:
    LvglUtils() = delete;

    LvglUtils(const LvglUtils &) = delete;

    LvglUtils operator=(const LvglUtils &) = delete;

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

    static lv_obj_t *createLvglDialog();

    static lv_obj_t *createLvglSysDialog();

    static lv_obj_t *createLvglProgress(lv_obj_t *parent);

    static lv_obj_t *createLvglChart(lv_obj_t *parent);

    static lv_obj_t *createLvglVideo(lv_obj_t *parent);
};
} // namespace tools
} // namespace lvglpp

#endif // LVPP_UTILS_H
