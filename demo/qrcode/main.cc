#include "TestPage.h"
#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include "core/tools/fonts/LvFontManager.h"

using namespace lvglpp;

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::tools::LvFontManager::enableFreetype(false);

    lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<TestPage>());

    return app.exec();
}
