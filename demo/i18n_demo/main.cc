#include "LangChange.h"
#include "LvTranslator.h"
#include "core/sys/SdlApplication.h"
#include "core/sys/StackView.h"
#include "session.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    WebsocketSession::Instance().Init();

    lvglpp::LvTranslator::init();

    lvglpp::LvTranslator::load(lvglpp::LANGUAGE::RUSSIAN);

    lvglpp::sys::StackView::getInstance()->pushView(std::make_shared<LangChangePage>());

    return app.exec();
}
