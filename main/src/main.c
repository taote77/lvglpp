
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "glob.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

 
/**********************
 *  STATIC VARIABLES
 **********************/

/********************** 
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern void freertos_main(void);

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#include "drivers/wayland/lv_wayland.h"


uint16_t window_width =  800;
uint16_t window_height = 600;

bool fullscreen = false;
bool maximize = false;

/* Currently, the wayland driver calls lv_timer_handler internaly */
void lv_linux_run_loop(void)
{

    bool completed;

    /* Handle LVGL tasks */
    while (1) {

        completed = lv_wayland_timer_handler();

        if (completed) {
            /* wait only if the cycle was completed */
            usleep(LV_DEF_REFR_PERIOD * 1000);
        }

        /* Run until the last window closes */
        if (!lv_wayland_window_is_open(NULL)) {
            break;
        }
    }

}

void lv_linux_disp_init(void)
{
    lv_display_t *disp;
    lv_group_t *g;

    disp = lv_wayland_window_create(window_width, window_height,
            "LVGL Simulator", NULL);

    if (fullscreen) {
            lv_wayland_window_set_fullscreen(disp, fullscreen);
    } else if (maximize) {
            lv_wayland_window_set_maximized(disp, maximize);
    }

    g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(lv_wayland_get_keyboard(disp), g);
    lv_indev_set_group(lv_wayland_get_pointeraxis(disp), g);

}

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  // lv_wayland_init();
  lv_linux_disp_init();


  #if LV_USE_OS == LV_OS_NONE
 
//   lv_demo_widgets();

  lv_demo_ebike();

  // while(1) {
  //   /* Periodically call the lv_task handler.
  //    * It could be done in a timer interrupt or an OS task too.*/
  //   lv_timer_handler();
  //   usleep(5 * 1000);
  // }

  lv_linux_run_loop();

  #elif LV_USE_OS == LV_OS_FREERTOS

  /* Run FreeRTOS and create lvgl task */
  freertos_main();  

  #endif

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

