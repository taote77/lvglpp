#include "Widget.h"
#include "core/sys/SdlApplication.h"
#include "core/sys/StackView.h"
#include "thorvg.h"
#include <pthread.h>
#include <unistd.h>

void thorvg_init()
{
    tvg::Initializer::init(tvg::CanvasEngine::Sw, 0); // 使用软件渲染引擎
}

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp())
    {
        return -1;
    }

    thorvg_init();

    lvglpp::sys::StackView::getInstance().pushView(std::make_shared<Widget>());

    return app.exec();
}
