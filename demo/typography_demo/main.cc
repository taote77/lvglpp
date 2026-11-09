#include "LvTranslator.h"
#include "TypographyDemoActivity.h"

#include "core/sys/Application.h"
#include "core/sys/StackView.h"

#include <memory>

int main(int argc, char *argv[])
{
    auto app = lvglpp::sys::createApplication(argc, argv);
    if (!app || !app->initialize())
    {
        return -1;
    }

    typography_demo::initTranslations();
    lvglpp::sys::StackView::getInstance().pushView(
        std::make_shared<typography_demo::TypographyDemoActivity>());

    return app->exec();
}
