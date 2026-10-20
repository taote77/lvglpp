#include "LangChange.h"
#include "LvTranslator.h"
#include "core/sys/Application.h"
#include "core/sys/StackView.h"
#include "session.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    auto app = lvglpp::sys::createApplication(argc, argv);

    if (!app || !app->initialize())
    {
        return -1;
    }

    WebsocketSession::Instance().Init();

    lvglpp::LvTranslator::init();

    lvglpp::LvTranslator::load(lvglpp::LANGUAGE::RUSSIAN);

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<LangChangePage>());

    return app->exec();
}
