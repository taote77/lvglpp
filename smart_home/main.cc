#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include "page/home/HomePage.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<HomePage>());

    return app.exec();
}
