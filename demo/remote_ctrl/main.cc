#include "RemoteClient.h"
#include "core/sys/SdlSimulateApplication.h"
#include "core/sys/TaskStack.h"
#include "session.h"
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    WebsocketSession::Instance().Init();

    lvglpp::sys::TaskStack::getInstance()->pushView(std::make_shared<RemoteClient>());

    return app.exec();
}
