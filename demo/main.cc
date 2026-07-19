#include "BindDemo.h"

#include "core/sys/SdlApplication.h"
#include "core/sys/StackView.h"

// #include <pthread.h>
// #include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<BindDemo>());

    return app.exec();
}
