#include "LangChange.h"
#include "LvTranslator.h"
#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    lvglpp::LvTranslator::Instance().init();

    lvglpp::LvTranslator::Instance().load(lvglpp::LANGUAGE::RUSSIAN);

    lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<LangChangePage>());

    return app.exec();
}
