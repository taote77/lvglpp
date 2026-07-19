#include "TestPage.h"
#include "core/sys/app/SdlApplication.h"
#include "core/sys/StackView.h"
#include "core/tools/fonts/LvFontManager.h"

using namespace lvglpp;

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::tools::LvFontManager::enableFreetype(false);

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<TestPage>());

    return app.exec();
}
