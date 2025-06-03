#include "BindDemo.h"

#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include "thorvg.h"
#include <pthread.h>
#include <unistd.h>

void thorvg_init()
{
    tvg::Initializer::init(tvg::CanvasEngine::Sw, 0); // 使用软件渲染引擎
}

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlSimulateApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    thorvg_init();

    lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<BindDemo>());

    return app.exec();
}
