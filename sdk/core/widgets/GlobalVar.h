/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-17

Class:定义全局变量。如颜色

Description:

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_GLOBALVAR_H
#define LVGL_XFJ_DEMO_GLOBALVAR_H
#include <cstdint>

// 品牌色 - 主色
const uint32_t CLR_PRIMARY = 0x4986FF;
const uint32_t CLR_ON_PRIMARY = 0xFFFFFF;
const uint32_t CLR_PRIMARY_CONTAINER = 0x2F58A9;
const uint32_t CLR_ON_PRIMARY_CONTAINER = 0xD5DDFB;

// 品牌色 - 次要色
const uint32_t CLR_SECONDARY = 0x7C90A7;
const uint32_t CLR_ON_SECONDARY = 0xFFFFFF;
const uint32_t CLR_SECONDARY_CONTAINER = 0x5D7090;
const uint32_t CLR_ON_SECONDARY_CONTAINER = 0xE6EAF8;

// 功能色 - 错误
const uint32_t CLR_ERROR = 0xED5855;
const uint32_t CLR_ON_ERROR = 0xFFFFFF;
const uint32_t CLR_ERROR_CONTAINER = 0xB33330;
const uint32_t CLR_ON_ERROR_CONTAINER = 0xFFD4D3;

// 功能色 - 警告
const uint32_t CLR_WARNING = 0xF5A34A;
const uint32_t CLR_ON_WARNING = 0xFFFFFF;
const uint32_t CLR_WARNING_CONTAINER = 0xA9700D;
const uint32_t CLR_ON_WARNING_CONTAINER = 0xFFECD8;

// 功能色 - 成功
const uint32_t CLR_SUCCESS = 0x0AC882;
const uint32_t CLR_ON_SUCCESS = 0xFFFFFF;
const uint32_t CLR_SUCCESS_CONTAINER = 0x21845F;
const uint32_t CLR_ON_SUCCESS_CONTAINER = 0xD4FFEF;

// 功能色 - 信息
const uint32_t CLR_INFO = 0x34BAEF;
const uint32_t CLR_ON_INFO = 0xFFFFFF;
const uint32_t CLR_INFO_CONTAINER = 0x047DA2;
const uint32_t CLR_ON_INFO_CONTAINER = 0xD2F5FF;

// 中性色
const uint32_t CLR_SURFACE = 0x444E5B;
const uint32_t CLR_SURFACE_DIM = 0x1E1E28;
const uint32_t CLR_SURFACE_BRIGHT = 0x7A8494;

// 容器色
const uint32_t CLR_SURFACE_CONTAINER_LOWEST = 0x2C3138;
const uint32_t CLR_SURFACE_CONTAINER_LOW = 0x3E444D;
const uint32_t CLR_SURFACE_CONTAINER = 0x647384;
const uint32_t CLR_SURFACE_CONTAINER_HIGH = 0x5A6676;
const uint32_t CLR_SURFACE_CONTAINER_HIGHEST = 0x73869B;

// 表面文字色
const uint32_t CLR_ON_SURFACE = 0xFFFFFF;
const uint32_t CLR_ON_SURFACE_VARIANT_0 = 0xE3EAEE;
const uint32_t CLR_ON_SURFACE_VARIANT_1 = 0xABB5C2;
const uint32_t CLR_ON_SURFACE_VARIANT_2 = 0x95A6C0;

// 描边色
const uint32_t CLR_OUTLINE = 0x707F90;
const uint32_t CLR_OUTLINE_VARIANT = 0x6B7580;




//old

//const uint32_t CLR_MAIN_BACKGROUND=0XEAEEF4;
const uint32_t CLR_TEXT_PRIMARY_BLACK=0x161616;
const uint32_t CLR_PRIMARY_BRIGHT=0xFFFFFF;


const uint32_t CLR_BACKGROUND=0x1A1D22;
const uint32_t CLR_PRIMARY_BLACK=0x161616;
const uint32_t CLR_BUTTON_BG_BLACK2=0xDDDEE2;
const uint32_t CLR_PRIMARY_BLUE=0x3760EA;
const uint32_t CLR_PRIMARY_DARK_BLUE=0x0025AC;
const uint32_t CLR_PRIMARY_BTN_BORDER=0xC0C1D0;
const uint32_t CLR_PRIMARY_DARK_BG=0x2C3138;
const uint32_t CLR_PRIMARY_BLACK_HOVER=0xBCC3CC;
const uint32_t CLR_PRIMARY_DIS_BG=0x6B7580;
const uint32_t CLR_PRIMARY_DIS_TEXT=0xEDEFF2;
const uint32_t CLR_PRIMARY_DIS_TEXT_PRESS=0x2C3138;
const uint32_t CLR_SECONDARY_DANGER_MASTER=0xE55353;
//const uint32_t CLR_SUCCESS=0x06AD85;
//const uint32_t CLR_WARNING=0xF19F3B;
const uint32_t CLR_WARNING_DARK=0xCA2525;
const uint32_t CLR_PRIMARY_BLUE_DIS_TEXT=0xBAC8F8;
const uint32_t CLR_PRIMARY_BLUE_DIS_BG=0xF7F9FF;
const uint32_t CLR_KEYBOARD_BG=0xE1E5EA;
const uint32_t CLR_BANNER_INFO_BG=0xE3E8F8;
const uint32_t CLR_BANNER_WARN_BG=0xFCECD8;
const uint32_t CLR_BANNER_ERROR_BG=0xE55353;
#endif //LVGL_XFJ_DEMO_GLOBALVAR_H
