
#include "LvglUtils.h"

namespace lvglpp {
namespace tools {

lv_obj_t *LvglUtils::createLvglItem(lv_obj_t *parent)
{
    lv_obj_t *item = createLvglScrolable(parent);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    return item;
}

lv_obj_t *LvglUtils::createLvglScrolable(lv_obj_t *parent)
{
    lv_obj_t *item = lv_obj_create(parent);
    lv_obj_set_style_pad_all(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item, lv_color_hex(0X444E5B), LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(item, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    return item;
}

lv_obj_t *LvglUtils::createLvglImage(lv_obj_t *parent)
{
    return lv_img_create(parent);
}

lv_obj_t *LvglUtils::createLvglLabel(lv_obj_t *parent)
{
    return lv_label_create(parent);
}

lv_obj_t *LvglUtils::createLvglSpinBox(lv_obj_t *parent)
{
    return lv_spinbox_create(parent);
}

lv_obj_t *LvglUtils::createLvglGif(lv_obj_t *parent)
{
    return lv_gif_create(parent);
}

lv_obj_t *LvglUtils::createLvglDialog()
{
    lv_obj_t *item = lv_obj_create(lv_layer_top());
    lv_obj_set_style_pad_all(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(item, 0, LV_STATE_DEFAULT);
    return item;
}

lv_obj_t *LvglUtils::createLvglProgress(lv_obj_t *parent)
{
    return lv_bar_create(parent);
}

lv_obj_t *LvglUtils::createLvglSysDialog()
{
    lv_obj_t *item = lv_obj_create(lv_layer_sys());
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(item, 0, LV_STATE_DEFAULT);
    return item;
}

lv_obj_t *LvglUtils::createLvglChart(lv_obj_t *parent)
{
    return lv_chart_create(parent);
}

lv_obj_t *LvglUtils::createLvglVideo(lv_obj_t *parent)
{
    return lv_ffmpeg_player_create(parent);
}
} // namespace tools
} // namespace lvglpp
