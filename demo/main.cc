#include "BindDemo.h"

#include "core/sys/SdlSimulateApplication.h"
#include "core/sys/TaskStack.h"

// #include <pthread.h>
// #include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::sys::TaskStack::getInstance()->pushView(std::make_shared<BindDemo>());

    return app.exec();
}
