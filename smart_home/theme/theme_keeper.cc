#include "theme_keeper.h"
#include <src/misc/lv_style.h>

using namespace form::design;

ThemeKeeper &ThemeKeeper::Instance()
{
    static ThemeKeeper theme_keeper;

    return theme_keeper;
}


ThemeKeeper::ThemeKeeper()
{
    init();
}


void ThemeKeeper::SetTheme(const std::string &theme_name)
{
}

lv_style_t &ThemeKeeper::StyleTaskCard()
{
    return _style_task_card;
}


lv_style_t &ThemeKeeper::StyleCardGradu()
{
    return _style_card_gradu;
}

lv_style_t &ThemeKeeper::StyleButton()
{
    return _style_btn;
}


void ThemeKeeper::init()
{

    // card style 渐变
    lv_style_init(&_style_card_gradu);
    // 设置圆角半径
    lv_style_set_radius(&_style_card_gradu, 16);
    // 设置边框
    lv_style_set_border_width(&_style_card_gradu, 2);
    // lv_style_set_border_color(&style, lv_color_hex(0x707F90));
    lv_style_set_border_color(&_style_card_gradu, lv_color_hex(CLR_ON_SURFACE_VARIANT_0));
    lv_style_set_border_side(&_style_card_gradu, LV_BORDER_SIDE_FULL);
    // 设置背景颜色
    lv_style_set_bg_color(&_style_card_gradu, lv_color_hex(CLR_OUTLINE));

    // 设置阴影（LVGL没有直接的阴影设置，这里可以用渐变来模拟）
    // 这里只是简单示例，实际阴影效果可能需要更复杂的实现
    lv_style_set_shadow_width(&_style_card_gradu, 24);
    lv_style_set_shadow_color(&_style_card_gradu, lv_color_mix(lv_color_hex(CLR_GRADU), lv_color_black(), 0.2 * 255));
    lv_style_set_shadow_offset_y(&_style_card_gradu, -4);

    // task card

    // border-radius: 16px;
    // border: 2px solid var(--outline-outline-36, #707F90);
    // background: var(--Surface-surface-10_, #444E5B);
    // box-shadow: 0px -4px 24px 0px rgba(176, 183, 208, 0.20);
    lv_style_init(&_style_task_card);
    lv_style_set_radius(&_style_task_card, 16);
    lv_style_set_border_width(&_style_task_card, 2);
    // lv_style_set_border_color(&_style_task_card, lv_color_hex(0x707F90));
    lv_style_set_border_color(&_style_task_card, lv_color_hex(CLR_ON_SURFACE));
    lv_style_set_shadow_offset_x(&_style_task_card, 0);
    lv_style_set_shadow_offset_y(&_style_task_card, -4);
    lv_style_set_bg_color(&_style_task_card, lv_color_hex(CLR_SURFACE));
    lv_style_set_shadow_color(&_style_task_card, lv_color_hex(CLR_GRADU));
    lv_style_set_shadow_opa(&_style_task_card, LV_OPA_20);


    // 按钮样式
    lv_style_init(&_style_btn);
    // 设置圆角半径
    lv_style_set_radius(&_style_btn, 12);
    // 设置渐变背景
    static lv_grad_dsc_t grad_dsc;
    // lv_grad_vertical_init(&grad_dsc);
    static lv_color_t colors[2];                       // 定义颜色数组
    colors[0] = lv_color_hex(CLR_PROGRESS_NORMAL_FILL);// 起始颜色
    colors[1] = lv_color_hex(0x3360B7);                // 结束颜色
    lv_opa_t opas[] = {1, 1};
    lv_grad_init_stops(&grad_dsc, colors, opas, nullptr, 2);

    lv_style_set_bg_grad(&_style_btn, &grad_dsc);
    lv_style_set_bg_grad_dir(&_style_btn, LV_GRAD_DIR_LINEAR);
}
