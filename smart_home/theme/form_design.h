#ifndef UI_FORM_DESIGN
#define UI_FORM_DESIGN

/**************************************************************************

Class:定义全局变量。如颜色

Description:

**************************************************************************/


#include <cstdint>

namespace form::design
{

    using LColorType = uint32_t;

    enum class DesignStyle
    {
        DARK = 0,
        SHALLOW = 1,
    };

    static LColorType ThemeColor(LColorType color, DesignStyle style = DesignStyle::DARK)
    {
        if (style == DesignStyle::DARK)
        {
            return color;
        }

        return color;
    };


    // 品牌色 - 主色
    const uint32_t CLR_PRIMARY = 0x4986FF;             // #4986FF
    const uint32_t CLR_ON_PRIMARY = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_PRIMARY_CONTAINER = 0x2F58A9;   // #2F58A9
    const uint32_t CLR_ON_PRIMARY_CONTAINER = 0xD5DDFB;// #D5DDFB


    // 品牌色 - 次要色
    const uint32_t CLR_SECONDARY = 0x7C90A7;             // #7C90A7
    const uint32_t CLR_ON_SECONDARY = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_SECONDARY_CONTAINER = 0x5D7090;   // #5D7090
    const uint32_t CLR_ON_SECONDARY_CONTAINER = 0xE6EAF8;// #E6EAF8

    // 功能色 - 错误
    const uint32_t CLR_ERROR = 0xED5855;             // #ED5855
    const uint32_t CLR_ON_ERROR = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_ERROR_CONTAINER = 0xB33330;   // #B33330
    const uint32_t CLR_WARN_CONTAINER = 0xF19F3B;    // #F19F3B
    const uint32_t CLR_ON_ERROR_CONTAINER = 0xFFD4D3;// #FFD4D3

    // 功能色 - 警告
    const uint32_t CLR_WARNING = 0xF5A34A;             // #F5A34A
    const uint32_t CLR_ON_WARNING = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_WARNING_CONTAINER = 0xA9700D;   // #A9700D
    const uint32_t CLR_ON_WARNING_CONTAINER = 0xFFECD8;// #FFECD8

    // 功能色 - 成功
    const uint32_t CLR_SUCCESS = 0x0AC882;             // #0AC882
    const uint32_t CLR_ON_SUCCESS = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_SUCCESS_CONTAINER = 0x21845F;   // #21845F
    const uint32_t CLR_ON_SUCCESS_CONTAINER = 0xD4FFEF;// #D4FFEF

    // 功能色 - 信息
    const uint32_t CLR_INFO = 0x34BAEF;             //  #34BAEF
    const uint32_t CLR_ON_INFO = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_INFO_CONTAINER = 0x047DA2;   // #047DA2
    const uint32_t CLR_ON_INFO_CONTAINER = 0xD2F5FF;// #D2F5FF

    // 中性色
    const uint32_t CLR_SURFACE = 0x444E5B;       // #444E5B
    const uint32_t CLR_SURFACE_DIM = 0x1E1E28;   // #1E1E28
    const uint32_t CLR_SURFACE_BRIGHT = 0x7A8494;// #7A8494

    // 容器色
    const uint32_t CLR_SURFACE_CONTAINER_LOWEST = 0x2C3138;// #2C3138
    const uint32_t CLR_SURFACE_CONTAINER_LOW = 0x3E444D;   // #3E444D
    const uint32_t CLR_SURFACE_CONTAINER = 0x647384;       // #647384
    const uint32_t CLR_SURFACE_CONTAINER_HIGH = 0x5A6676;  // #5A6676
    const uint32_t CLR_SURFACE_CONTAINER_HIGHEST = 0x73869B;

    const uint32_t CLR_SURFACE_CONTAINER_1 = 0x677687;    // #677687
    const uint32_t CLR_SURFACE_DIS_CONTAINER_1 = 0x626A75;// #626A75


    // 表面文字色
    const uint32_t CLR_ON_SURFACE = 0xFFFFFF;          // #FFFFFF
    const uint32_t CLR_ON_SURFACE_VARIANT_0 = 0xE3EAEE;// #E3EAEE
    const uint32_t CLR_ON_SURFACE_VARIANT_1 = 0xABB5C2;// #ABB5C2;
    const uint32_t CLR_ON_SURFACE_VARIANT_2 = 0x95A6C0;// #95A6C0;

    // 描边色
    const uint32_t CLR_OUTLINE = 0x707F90;        // #707F90;
    const uint32_t CLR_OUTLINE_VARIANT = 0x6B7580;// #6B7580;

    const uint32_t CLR_MAIN_BACKGROUND = 0x3B424D;// #3B424D


    // 填充色
    const uint32_t CLR_PROGRESS_NORMAL_FILL = 0x6297FF;//  #6297FF
    const uint32_t CLR_PROGRESS_WARN_FILL = 0xEE8A20;  // #EE8A20
    const uint32_t CLR_PROGRESS_ERROR_FILL = 0xE92E2B; // #E92E2B


    const uint32_t CLR_GREATER_BUTTON = 0x578BEF;

    const uint32_t CLR_BUTTON_GRADU_START = 0x6297FF;//  #6297FF
    const uint32_t CLR_BUTTON_GRADU_END = 0x3360B7;  // #3360B7

    // const uint32_t CLR_PROGRESS_WARN_FILL = 0x;

    const uint32_t CLR_GRADU = 0xB0B7D0;

    const uint32_t CLR_INDICATOR_SELECTED = 0xFFFFFF;  // #FFFFFF
    const uint32_t CLR_INDICATOR_UNSELECTED = 0xABB6C3;// #ABB6C3


    const uint32_t CLR_DIALOG_BG_COLOR = 0x000000;// #000000
    const uint32_t CLR_DIALOG_BG_MASK = 0x1A1D22; // #1A1D22 · 70%

    const double MASK_OPA_70 = 0.70F;// 70%


    // conf


    const uint32_t WIN_WIDTH = 800;
    const uint32_t WIN_HEIGHT = 480;
    const uint32_t TOP_BAR_HEIGHT = 80;
    const uint32_t RADIUS_8 = 8;
    const uint32_t RADIUS_10 = 10;
    const uint32_t RADIUS_12 = 12;
    const uint32_t RADIUS_28 = 28;

    constexpr uint32_t BTN_WIDTH_396 = 396;
    constexpr uint32_t BTN_WIDTH_296 = 296;
    constexpr uint32_t BTN_WIDTH_242 = 242;
    constexpr uint32_t BTN_HEIGHT_64 = 64;


    //old

    //const uint32_t CLR_MAIN_BACKGROUND=0XEAEEF4;
    const uint32_t CLR_TEXT_PRIMARY_BLACK = 0x161616;
    const uint32_t CLR_PRIMARY_BRIGHT = 0xFFFFFF;


    const uint32_t CLR_BACKGROUND = 0x3B424D;
    const uint32_t CLR_PRIMARY_BLACK = 0x161616;
    const uint32_t CLR_BUTTON_BG_BLACK2 = 0xDDDEE2;
    const uint32_t CLR_PRIMARY_BLUE = 0x3760EA;
    const uint32_t CLR_PRIMARY_DARK_BLUE = 0x0025AC;
    const uint32_t CLR_PRIMARY_BTN_BORDER = 0xC0C1D0;
    const uint32_t CLR_PRIMARY_DARK_BG = 0x2C3138;
    const uint32_t CLR_PRIMARY_BLACK_HOVER = 0xBCC3CC;
    const uint32_t CLR_PRIMARY_DIS_BG = 0x6B7580;
    const uint32_t CLR_PRIMARY_DIS_TEXT = 0xEDEFF2;
    const uint32_t CLR_PRIMARY_DIS_TEXT_PRESS = 0x2C3138;// #2C3138
    const uint32_t CLR_SECONDARY_DANGER_MASTER = 0xE55353;
    //const uint32_t CLR_SUCCESS=0x06AD85;
    //const uint32_t CLR_WARNING=0xF19F3B;
    const uint32_t CLR_WARNING_DARK = 0xCA2525;
    const uint32_t CLR_PRIMARY_BLUE_DIS_TEXT = 0xBAC8F8;
    const uint32_t CLR_PRIMARY_BLUE_DIS_BG = 0xF7F9FF;
    const uint32_t CLR_KEYBOARD_BG = 0xE1E5EA;
    const uint32_t CLR_BANNER_INFO_BG = 0xE3E8F8;
    const uint32_t CLR_BANNER_WARN_BG = 0xFCECD8;
    const uint32_t CLR_BANNER_ERROR_BG = 0xE55353;


}// namespace form::design

#endif// UI_FORM_DESIGN
