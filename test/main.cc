#include "glob.h"
// #include "lvgl/demos/lv_demos.h"
// #include "lvgl/examples/lv_examples.h"
#include "lvgl/lvgl.h"
#include "printer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define USE_WAYLAND

#ifdef USE_WAYLAND
#include "window_wayland.h"
#else
#include "window_framebuf"
#endif

int main(int argc, char **argv) {
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

#ifdef USE_WAYLAND
  lv_init();

  lv_linux_disp_init();

#else

#endif

  lv_demo_ebike();

#ifdef USE_WAYLAND
  lv_linux_run_loop();
#else

#endif

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
