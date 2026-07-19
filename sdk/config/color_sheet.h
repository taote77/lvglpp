#ifndef LV_COLOR_SHEET_H
#define LV_COLOR_SHEET_H

#include <cstdint>

namespace form::design {

// 品牌色 - 主色
const uint32_t CLR_PRIMARY              = 0x4986FF; // #4986FF
const uint32_t CLR_ON_PRIMARY           = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_PRIMARY_CONTAINER    = 0x11225D; // #11225D
const uint32_t CLR_ON_PRIMARY_CONTAINER = 0xD5DDFB; // #D5DDFB

// 品牌色 - 次要色
const uint32_t CLR_SECONDARY              = 0x191D2A; // #191D2A
const uint32_t CLR_ON_SECONDARY           = 0xD7DBE7; // #D7DBE7
const uint32_t CLR_SECONDARY_CONTAINER    = 0x485479; // #485479
const uint32_t CLR_ON_SECONDARY_CONTAINER = 0xE6EAF8; // #E6EAF8

// 功能色 - 错误
const uint32_t CLR_ERROR              = 0xE92E2B; // #E92E2B
const uint32_t CLR_ON_ERROR           = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_ERROR_CONTAINER    = 0x5D1211; // #5D1211
const uint32_t CLR_WARN_CONTAINER     = 0xF19F3B; // #F19F3B
const uint32_t CLR_ON_ERROR_CONTAINER = 0xFFD4D3; // #FFD4D3

// 功能色 - 警告
const uint32_t CLR_WARNING              = 0xF5A34A; // #F5A34A
const uint32_t CLR_ON_WARNING           = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_WARNING_CONTAINER    = 0x5D3811; // #5D3811
const uint32_t CLR_ON_WARNING_CONTAINER = 0xFFECD8; // #FFECD8

// 功能色 - 成功
const uint32_t CLR_SUCCESS              = 0x0AC882; // #0AC882
const uint32_t CLR_ON_SUCCESS           = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_SUCCESS_CONTAINER    = 0x115D42; // #115D42
const uint32_t CLR_ON_SUCCESS_CONTAINER = 0xD4FFEF; // #D4FFEF

// 功能色 - 信息
const uint32_t CLR_INFO              = 0x34BAEF; //  #34BAEF
const uint32_t CLR_ON_INFO           = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_INFO_CONTAINER    = 0x114B5D; // #114B5D
const uint32_t CLR_ON_INFO_CONTAINER = 0xD2F5FF; // #D2F5FF

// 中性色
const uint32_t CLR_SURFACE        = 0x12151E; // #12151E
const uint32_t CLR_SURFACE_DIM    = 0x1D1F29; // #1D1F29
const uint32_t CLR_SURFACE_BRIGHT = 0x313852; // #313852

// 容器色
const uint32_t CLR_SURFACE_CONTAINER_LOWEST  = 0x1D1F29; // #1D1F29
const uint32_t CLR_SURFACE_CONTAINER_LOW     = 0x191D2A; // #191D2A
const uint32_t CLR_SURFACE_CONTAINER         = 0x252B3E; // #252B3E
const uint32_t CLR_SURFACE_CONTAINER_HIGH    = 0x313852; // #313852
const uint32_t CLR_SURFACE_CONTAINER_HIGHEST = 0x3C4564;

const uint32_t CLR_SURFACE_CONTAINER_1     = 0x313852; // #313852
const uint32_t CLR_SURFACE_DIS_CONTAINER_1 = 0x3D3E4C; // #3D3E4C

// 表面文字色
const uint32_t CLR_ON_SURFACE           = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_ON_SURFACE_VARIANT_0 = 0xF3F4F7; // #F3F4F7
const uint32_t CLR_ON_SURFACE_VARIANT_1 = 0xD7DBE7; // #D7DBE7;
const uint32_t CLR_ON_SURFACE_VARIANT_2 = 0xB0B7D0; // #B0B7D0;

// 描边色
const uint32_t CLR_OUTLINE         = 0x485479; // #485479;
const uint32_t CLR_OUTLINE_VARIANT = 0x313852; // #313852;

const uint32_t CLR_MAIN_BACKGROUND = 0x191D2A; // #191D2A

// 填充色
const uint32_t CLR_PROGRESS_NORMAL_FILL = 0x6297FF; //  #6297FF
const uint32_t CLR_PROGRESS_WARN_FILL   = 0xEE8A20; // #EE8A20
const uint32_t CLR_PROGRESS_ERROR_FILL  = 0xE92E2B; // #E92E2B

const uint32_t CLR_GREATER_BUTTON = 0x578BEF;

const uint32_t CLR_BUTTON_GRADU_START = 0x6297FF; //  #6297FF
const uint32_t CLR_BUTTON_GRADU_END   = 0x3360B7; // #3360B7

// const uint32_t CLR_PROGRESS_WARN_FILL = 0x;

const uint32_t CLR_GRADU = 0xB0B7D0;

// 按钮色
const uint32_t CLR_WIDGET_BUTTON_BLUE     = 0x4986FF; // #4986FF;
const uint32_t CLR_WIDGET_BUTTON_BLUE_DIS = 0x3A67C0; //  #3A67C0

// const uint32_t CLR_WIDGET_BUTTON_GRAY = 0x3A67C0;//  #3A67C0
const uint32_t CLR_WIDGET_BUTTON_GRAY_DIS = 0x3A4462; //  #3A4462

const uint32_t CLR_INDICATOR_SELECTED   = 0xFFFFFF; // #FFFFFF
const uint32_t CLR_INDICATOR_UNSELECTED = 0xABB6C3; // #ABB6C3

const uint32_t CLR_DIALOG_BG_COLOR = 0x000000; // #000000
const uint32_t CLR_DIALOG_BG_MASK  = 0x1A1D22; // #1A1D22 · 70%

const uint32_t CLR_CHECKBOX_CONTAINER = 0x647384; // #647384
const uint32_t CLR_CHECKBOX_SELECTED  = 0x34BAEF; // #34BAEF;

const double MASK_OPA_70 = 0.70F; // 70%

// old

// const uint32_t CLR_MAIN_BACKGROUND=0XEAEEF4;
const uint32_t CLR_TEXT_PRIMARY_BLACK = 0x161616;
const uint32_t CLR_PRIMARY_BRIGHT     = 0xFFFFFF;

const uint32_t CLR_BACKGROUND              = 0x3B424D;
const uint32_t CLR_PRIMARY_BLACK           = 0x161616;
const uint32_t CLR_BUTTON_BG_BLACK2        = 0xDDDEE2;
const uint32_t CLR_PRIMARY_BLUE            = 0x3760EA;
const uint32_t CLR_PRIMARY_DARK_BLUE       = 0x0025AC;
const uint32_t CLR_PRIMARY_BTN_BORDER      = 0xC0C1D0;
const uint32_t CLR_PRIMARY_DARK_BG         = 0x2C3138;
const uint32_t CLR_PRIMARY_BLACK_HOVER     = 0xBCC3CC;
const uint32_t CLR_PRIMARY_DIS_BG          = 0x6B7580;
const uint32_t CLR_PRIMARY_DIS_TEXT        = 0xEDEFF2;
const uint32_t CLR_PRIMARY_DIS_TEXT_PRESS  = 0x2C3138; // #2C3138
const uint32_t CLR_SECONDARY_DANGER_MASTER = 0xE55353;
// const uint32_t CLR_SUCCESS=0x06AD85;
// const uint32_t CLR_WARNING=0xF19F3B;
const uint32_t CLR_WARNING_DARK          = 0xCA2525;
const uint32_t CLR_PRIMARY_BLUE_DIS_TEXT = 0xBAC8F8;
const uint32_t CLR_PRIMARY_BLUE_DIS_BG   = 0xF7F9FF;
const uint32_t CLR_KEYBOARD_BG           = 0xE1E5EA;
const uint32_t CLR_BANNER_INFO_BG        = 0xE3E8F8;
const uint32_t CLR_BANNER_WARN_BG        = 0xFCECD8;
const uint32_t CLR_BANNER_ERROR_BG       = 0xE55353;

} // namespace form::design

#endif // LV_COLOT_SHEET_H
