#include "core/sys/app/SdlApplication.h"
#include "core/sys/StackView.h"
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

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<HomePage>());

    return app.exec();
}
