#include "glob.h"
#include "printer.h"
#include <pthread.h>
#include <unistd.h>

#include "lvglpp_application.h"

int main(int argc, char *argv[])
{
    LvglApplication app(argc, argv);

    lv_demo_ebike(app.get_main_cont());

    return app.exec();
}
