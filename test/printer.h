
#ifndef LV_DEMO_EBIKE_H
#define LV_DEMO_EBIKE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"

/*Testing of dependencies*/
#if LV_USE_BIDI == 0
#error "BiDirectional text support is required. Enable it in lv_conf.h (LV_USE_BIDI 1)"
#endif

#if LV_USE_ARABIC_PERSIAN_CHARS == 0
#error "Arabic/Perisan character processing is required. Enable it in lv_conf.h (LV_USE_ARABIC_PERSIAN_CHARS 1)"
#endif


void lv_demo_ebike(void);


/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_EBIKE_H*/
