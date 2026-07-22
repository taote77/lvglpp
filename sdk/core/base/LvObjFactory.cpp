
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

lv_obj_t *LvObjFactory::createLvglDialog(lv_obj_t *parent)
{
    LV_UNUSED(parent);
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

lv_obj_t *LvObjFactory::createLvglSysDialog(lv_obj_t *parent)
{
    LV_UNUSED(parent);
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

lv_obj_t *LvObjFactory::createLvglQrCode(lv_obj_t *parent)
{
    return lv_qrcode_create(parent);
}

lv_obj_t *LvObjFactory::createLvglSlider(lv_obj_t *parent)
{
    return lv_slider_create(parent);
}

lv_obj_t *LvObjFactory::createLvglDropdown(lv_obj_t *parent)
{
    return lv_dropdown_create(parent);
}

lv_obj_t *LvObjFactory::createLvglBar(lv_obj_t *parent)
{
    return lv_bar_create(parent);
}

lv_obj_t *LvObjFactory::createLvglArc(lv_obj_t *parent)
{
    return lv_arc_create(parent);
}

lv_obj_t *LvObjFactory::createLvglTextArea(lv_obj_t *parent)
{
    return lv_textarea_create(parent);
}

lv_obj_t *LvObjFactory::createLvglKeyboard(lv_obj_t *parent)
{
    return lv_keyboard_create(parent);
}

lv_obj_t *LvObjFactory::createLvglTabView(lv_obj_t *parent)
{
    return lv_tabview_create(parent);
}

lv_obj_t *LvObjFactory::createLvglCanvas(lv_obj_t *parent)
{
    return lv_canvas_create(parent);
}

lv_obj_t *LvObjFactory::createLvglLed(lv_obj_t *parent)
{
    return lv_led_create(parent);
}

lv_obj_t *LvObjFactory::createLvglList(lv_obj_t *parent)
{
    return lv_list_create(parent);
}

lv_obj_t *LvObjFactory::createLvglLine(lv_obj_t *parent)
{
    return lv_line_create(parent);
}

lv_obj_t *LvObjFactory::createLvglRoller(lv_obj_t *parent)
{
    return lv_roller_create(parent);
}

lv_obj_t *LvObjFactory::createLvglCalendar(lv_obj_t *parent)
{
    return lv_calendar_create(parent);
}

lv_obj_t *LvObjFactory::createLvglTable(lv_obj_t *parent)
{
    return lv_table_create(parent);
}

} // namespace lvglpp::tools
