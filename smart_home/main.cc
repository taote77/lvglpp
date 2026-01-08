#include "core/sys/SdlApplication.h"
#include "core/sys/TaskStack.h"
#include "i18n/LvTranslator.h"
#include "page/home/HomePage.h"

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    LvTranslator::Instance().init();
    LvTranslator::Instance().load(LANGUAGE::CHINESE);

    lvglpp::sys::TaskStack::getInstance()->pushView(std::make_shared<HomePage>());

    return app.exec();
}
