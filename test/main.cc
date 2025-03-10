#include "glob.h"
#include "printer.h"
#include <pthread.h>
#include <unistd.h>
#include "core/sys/WaylandApplication.h"
#include "core/sys/Navigators.h"
#include "Home.h"

int main(int argc, char *argv[])
{
    heygears::sys::WaylandApplication app(argc, argv);

    // lv_demo_ebike(app.get_main_cont());
    // heygears::sys::Navigators::getInstance()->pushView(
    //         std::make_shared<heygears::ui::page::MakerPreparedActivity>());

    heygears::sys::Navigators::getInstance()->pushView(std::make_shared<Home>());

    return app.exec();
}
