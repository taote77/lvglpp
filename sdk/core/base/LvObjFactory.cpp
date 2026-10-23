
#include "LvObjFactory.h"
#include "core/style/Theme.h"

namespace lvglpp::tools {

namespace {

/// Shared default container style. Using a shared style (instead of local
/// style props) allows callers to override properties with Widget::applyStyle().
const core::Style &containerBaseStyle()
{
    // Process-lifetime style: LVGL objects reference it until lv_deinit().
    static const core::Style *style = [] {
        auto *s = new core::Style();
        s->padAll(0).radius(0).borderWidth(0).outline(0, 0x000000, 0);
        return s;
    }();
    return *style;
}

const core::Style &canvasStyle()
{
    static const core::Style *style = [] {
        auto *s = new core::Style(containerBaseStyle().clone());
        s->bgColor(core::Theme::instance().hex(core::ColorRole::SurfaceCanvas));
        return s;
    }();
    return *style;
}

} // namespace

lv_obj_t *LvObjFactory::createLvglItem(lv_obj_t *parent)
{
    lv_obj_t *item = createLvglScrolable(parent);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    return item;
}

lv_obj_t *LvObjFactory::createLvglScrolable(lv_obj_t *parent)
{
    lv_obj_t *item = lv_obj_create(parent);
    canvasStyle().applyTo(item);
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
    containerBaseStyle().applyTo(item);
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
    containerBaseStyle().applyTo(item);
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

lv_obj_t *LvObjFactory::createLvglSpinner(lv_obj_t *parent)
{
    return lv_spinner_create(parent);
}

lv_obj_t *LvObjFactory::createLvglImageButton(lv_obj_t *parent)
{
    return lv_imagebutton_create(parent);
}

lv_obj_t *LvObjFactory::createLvglMsgBox(lv_obj_t *parent)
{
    return lv_msgbox_create(parent);
}

lv_obj_t *LvObjFactory::createLvglScale(lv_obj_t *parent)
{
    return lv_scale_create(parent);
}

} // namespace lvglpp::tools
