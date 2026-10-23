#include "ImgDispActivity.h"
#include "core/sys/Application.h"
#include "core/sys/StackView.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    auto app = lvglpp::sys::createApplication(argc, argv);

    if (!app || !app->initialize())
    {
        return -1;
    }

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<ImgDispActivity>());

    return app->exec();
}
