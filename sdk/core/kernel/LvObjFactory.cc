
#include "LvObjFactory.h"

namespace lvglpp::tools {

lv_obj_t *LvObjFactory::createLvglItem(lv_obj_t *parent)
{
    lv_obj_t *item = createLvglScrolable(parent);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    return item;
}

lv_obj_t *LvObjFactory::createLvglScrolable(lv_obj_t *parent)
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

lv_obj_t *LvObjFactory::createLvglImage(lv_obj_t *parent)
{
    return lv_img_create(parent);
}

lv_obj_t *LvObjFactory::createLvglLabel(lv_obj_t *parent)
{
    return lv_label_create(parent);
}

lv_obj_t *LvObjFactory::createLvglSpinBox(lv_obj_t *parent)
{
    return lv_spinbox_create(parent);
}

lv_obj_t *LvObjFactory::createLvglGif(lv_obj_t *parent)
{
    return lv_gif_create(parent);
}

lv_obj_t *LvObjFactory::createLvglDialog()
{
    lv_obj_t *item = lv_obj_create(lv_layer_top());
    lv_obj_set_style_pad_all(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(item, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(item, 0, LV_STATE_DEFAULT);
    return item;
}

lv_obj_t *LvObjFactory::createLvglProgress(lv_obj_t *parent)
{
    return lv_bar_create(parent);
}

lv_obj_t *LvObjFactory::createLvglSysDialog()
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

lv_obj_t *LvObjFactory::createLvglChart(lv_obj_t *parent)
{
    return lv_chart_create(parent);
}

lv_obj_t *LvObjFactory::createLvglVideo(lv_obj_t *parent)
{
    return lv_ffmpeg_player_create(parent);
}

lv_obj_t *LvObjFactory::createLvglLottie(lv_obj_t *parent)
{
    return lv_lottie_create(parent);
}

// lv_obj_t *LvObjFactory::createLvglQrCode(lv_obj_t *parent)
// {
//     return lv_qrcode_create(parent);
// }

} // namespace lvglpp::tools
