#include "BindDemo.h"
#include "Home.h"
#include "core/sys/SdlApplication.h"
#include "core/sys/TaskStack.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::sys::TaskStack::getInstance()->pushView(std::make_shared<Home>());

    // lvglpp::sys::TaskStack::getInstance()->pushView(std::make_shared<BindDemo>());

    return app.exec();
}
