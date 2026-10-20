#include "TestPage.h"
#include "core/sys/Application.h"
#include "core/sys/StackView.h"
#include "core/tools/fonts/LvFontManager.h"

using namespace lvglpp;

int main(int argc, char *argv[])
{
    auto app = lvglpp::sys::createApplication(argc, argv);

    if (!app || !app->initialize())
    {
        return -1;
    }

    lvglpp::tools::LvFontManager::enableFreetype(false);

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<TestPage>());

    return app->exec();
}
