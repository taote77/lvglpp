## 渲染显示的本质


### Display

```c++
struct _lv_display_t {

    /** Horizontal resolution.*/
    int32_t hor_res;
    /** Vertical resolution.*/
    int32_t ver_res;

    /*---------------------
     * Buffering
     *--------------------*/
    /** MANDATORY: Write the internal buffer (draw_buf) to the display. 'lv_display_flush_ready()' has to be
     * called when finished*/
    lv_display_flush_cb_t flush_cb;

    /**
     * Used to wait while flushing is ready.
     * It can do any complex logic to wait, including semaphores, mutexes, polling flags, etc.
     * If not set `flushing` flag is used which can be cleared with `lv_display_flush_ready()` */
    lv_display_flush_wait_cb_t flush_wait_cb;

    /** 1: flushing is in progress. (It can't be a bit field because when it's cleared from IRQ
     * Read-Modify-Write issue might occur) */
    volatile int flushing;

    /** 1: It was the last chunk to flush. (It can't be a bit field because when it's cleared
     * from IRQ Read-Modify-Write issue might occur) */
    volatile int flushing_last;
    volatile uint32_t last_area         : 1; /**< 1: last area is being rendered */
    volatile uint32_t last_part         : 1; /**< 1: last part of the current area is being rendered */

    lv_display_render_mode_t render_mode;
    uint32_t antialiasing : 1;       /**< 1: anti-aliasing is enabled on this display.*/
    uint32_t tile_cnt     : 8;       /**< Divide the display buffer into these number of tiles */
    uint32_t stride_is_auto : 1;     /**< 1: The stride of the buffers was not set explicitly. */


    /** Invalidated (marked to redraw) areas*/
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint32_t inv_p;
    int32_t inv_en_cnt;

    /** Double buffer sync areas (redrawn during last refresh) */
    lv_ll_t sync_areas;

    lv_draw_buf_t _static_buf1; /**< Used when user pass in a raw buffer as display draw buffer */
    lv_draw_buf_t _static_buf2;
    /*---------------------
     * Layer
     *--------------------*/
    lv_layer_t * layer_head;
    void (*layer_init)(lv_display_t * disp, lv_layer_t * layer);
    void (*layer_deinit)(lv_display_t * disp, lv_layer_t * layer);

    /*---------------------
     * Screens
     *--------------------*/

    /** Screens of the display*/
    lv_obj_t ** screens;    /**< Array of screen objects.*/
    lv_obj_t * sys_layer;   /**< @see lv_display_get_layer_sys*/
    lv_obj_t * top_layer;   /**< @see lv_display_get_layer_top*/
    lv_obj_t * act_scr;     /**< Currently active screen on this display*/
    lv_obj_t * bottom_layer;/**< @see lv_display_get_layer_bottom*/
    lv_obj_t * prev_scr;    /**< Previous screen. Used during screen animations*/
    lv_obj_t * scr_to_load; /**< The screen prepared to load in lv_screen_load_anim*/
    uint32_t screen_cnt;
    uint8_t draw_prev_over_act  : 1;/** 1: Draw previous screen over active screen*/
    uint8_t del_prev  : 1;  /** 1: Automatically delete the previous screen when the screen load animation is ready*/

    /*---------------------
     * Others
     *--------------------*/
    lv_event_list_t event_list;

    /** A timer which periodically checks the dirty areas and refreshes them*/
    lv_timer_t * refr_timer;

    /*Miscellaneous data*/
    uint32_t last_activity_time;        /**< Last time when there was activity on this display*/

    /** The area being refreshed*/
    lv_area_t refreshed_area;
    uint32_t vsync_count;

};

```


typedef enum {
    /**
     * Use the buffer(s) to render the screen is smaller parts.
     * This way the buffers can be smaller then the display to save RAM. At least 1/10 screen size buffer(s) are recommended.

     * 将屏幕分成多个小区域逐块渲染
     * 缓冲区可以小于屏幕大小,节省内存
     * 推荐缓冲区大小: 至少屏幕的 1/10
     */
    LV_DISPLAY_RENDER_MODE_PARTIAL,

    /**
     * The buffer(s) has to be screen sized and LVGL will render into the correct location of the buffer.
     * This way the buffer always contain the whole image. Only the changed ares will be updated.
     * With 2 buffers the buffers' content are kept in sync automatically and in flush_cb only address change is required.
     * 
     * 缓冲区必须等于屏幕大小
     * 缓冲区始终包含完整画面
     * 只更新改变的区域(部分刷新)
     * 双缓冲模式: 缓冲区自动同步,flush_cb 只需切换地址

     */
    LV_DISPLAY_RENDER_MODE_DIRECT,

    /**
     * Always redraw the whole screen even if only one pixel has been changed.
     * With 2 buffers in flush_cb only an address change is required.

     * 每次都重绘整个屏幕,即使只改变一个像素
     * 缓冲区必须等于屏幕大小
     * 双缓冲模式: flush_cb 只需切换地址
     */
    LV_DISPLAY_RENDER_MODE_FULL,
} lv_display_render_mode_t;




## draw 
## 脏区渲染
屏幕内容缓冲区
  screen->contents   |   buffers

##  Buffing Mode


https://docs.lvgl.io/8/porting/display.html

https://docs.lvgl.io/8/overview/drawing.html#mechanism-of-screen-refreshing

缓存模式


Buffering modes
There are several settings to adjust the number draw buffers and buffering/refreshing modes.

You can measure the performance of different configurations using the benchmark example.

One buffer
If only one buffer is used LVGL draws the content of the screen into that draw buffer and sends it to the display. LVGL then needs to wait until the content of the buffer is sent to the display before drawing something new in it.

Two buffers
If two buffers are used LVGL can draw into one buffer while the content of the other buffer is sent to the display in the background. DMA or other hardware should be used to transfer data to the display so the MCU can continue drawing. This way, the rendering and refreshing of the display become parallel operations.

Full refresh
In the display driver () enabling the bit will force LVGL to always redraw the whole screen. This works in both one buffer and two buffers modes. If is enabled and two screen sized draw buffers are provided, LVGL's display handling works like "traditional" double buffering. This means the callback only has to update the address of the framebuffer ( parameter). This configuration should be used if the MCU has an LCD controller peripheral and not with an external display controller (e.g. ILI9341 or SSD1963) accessed via serial link. The latter will generally be too slow to maintain high frame rates with full screen redraws.lv_disp_drv_tfull_refreshfull_refreshflush_cbcolor_p

Direct mode
If the flag is enabled in the display driver LVGL will draw directly into a screen sized frame buffer. That is the draw buffer(s) needs to be screen sized. It this case will be called only once when all dirty areas are redrawn. With the frame buffer always contains the current frame as it should be displayed on the screen. If 2 frame buffers are provided as draw buffers LVGL will alter the buffers but always draw only the dirty areas. Therefore the 2 buffers needs to synchronized in like this:direct_modeflush_cbdirect_modeflush_cb

Display the frame buffer pointed by color_p

Copy the redrawn areas from to the other buffer.color_p

The get the redrawn areas to copy use the following functions returns the display being refreshed contains the invalidated areas if 1 that area was joined into another one and should be ignored number of valid elements in _lv_refr_get_disp_refreshing()disp->inv_areas[LV_INV_BUF_SIZE]disp->inv_area_joined[LV_INV_BUF_SIZE]disp->inv_pinv_areas




flush_cb a callback function to copy a buffer's content to a specific area of the display. needs to be called when flushing is ready. LVGL might render the screen in multiple chunks and therefore call multiple times. To see if the current one is the last chunk of rendering use .lv_disp_flush_ready(&disp_drv)flush_cblv_disp_flush_is_last(&disp_drv)


Some other optional callbacks to make it easier and more optimal to work with monochrome, grayscale or other non-standard RGB displays:

rounder_cb Round the coordinates of areas to redraw. E.g. a 2x2 px can be converted to 2x8. It can be used if the display controller can refresh only areas with specific height or width (usually 8 px height with monochrome displays).

set_px_cb a custom function to write the draw buffer. It can be used to store the pixels more compactly in the draw buffer if the display has a special color format. (e.g. 1-bit monochrome, 2-bit grayscale etc.) This way the buffers used in can be smaller to hold only the required number of bits for the given area size. Note that rendering with is slower than normal rendering.lv_disp_draw_buf_tset_px_cb

monitor_cb A callback function that tells how many pixels were refreshed and in how much time. Called when the last chunk is rendered and sent to the display.

clean_dcache_cb A callback for cleaning any caches related to the display.

render_start_cb A callback function that notifies the display driver that rendering has started. It also could be used to wait for VSYNC to start rendering. It's useful if rendering is faster than a VSYNC period.
