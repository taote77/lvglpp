#include <pthread.h>
#include <unistd.h>
#include "core/sys/SdlSimulateApplication.h"
#include "core/sys/Navigators.h"
#include "Home.h"

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp()) {
        return -1;
    }

    lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<Home>());

    return app.exec();
}
