#ifndef LVGLPP_APPLICATION
#define LVGLPP_APPLICATION

#include "lvgl/lvgl.h"

class LvglApplication
{
public:
    LvglApplication(int argc, char *argv[]);

    ~LvglApplication();

    lv_obj_t *get_main_cont(void);

    int exec();

protected:
    void lv_linux_run_loop(void);

    void lv_linux_disp_init(void);

private:
    lv_obj_t *_main_container;
};

#endif // LVGLPP_APPLICATION