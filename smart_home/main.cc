#include "core/sys/SdlSimulateApplication.h"
#include "core/sys/TaskStack.h"
#include "page/home/HomePage.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::sys::TaskStack::getInstance()->pushView(std::make_shared<HomePage>());

    return app.exec();
}
