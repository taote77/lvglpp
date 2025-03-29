#include "BindDemo.h"
#include "Home.h"
#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<Home>());

    // lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<BindDemo>());

    return app.exec();
}
