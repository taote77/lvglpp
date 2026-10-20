#include "LvglLuaBind.h"
#include "core/sys/Application.h"
#include "core/sys/BaseActivity.h"
#include <iostream>
#include <memory>

using namespace lvglpp;
using namespace lvglpp::lua;

class LuaUIApplication : public sys::Application
{
public:
    LuaUIApplication(int argc, char *argv[]) : Application(argc, argv), lua_bind_(std::make_unique<LvglLuaBind>())
    {}

    ~LuaUIApplication() override
    {
        lua_bind_->shutdown();
    }

protected:
    bool onInit() override
    {
        setEnableStackView(false);
        lua_bind_->initialize();

        std::cout << "Lua UI Application initialized" << std::endl;
        std::cout << "Loading Lua script..." << std::endl;

        // applicationDirPath() is the current working directory (build/target/bin).
        std::string exe_path    = Application::applicationDirPath();
        std::string script_path = exe_path + "/simple_demo.lua";

        if (!lua_bind_->executeFile(script_path))
        {
            std::cerr << "Failed to load Lua script" << std::endl;
            return false;
        }

        std::cout << "Lua script loaded successfully" << std::endl;
        std::cout << "Window size: 800x480" << std::endl;
        return true;
    }

    void onShutdown() override
    {
        if (lua_bind_)
        {
            lua_bind_->shutdown();
        }
    }

private:
    std::unique_ptr<LvglLuaBind> lua_bind_;
};

int main(int argc, char *argv[])
{
    try
    {
        LuaUIApplication app(argc, argv);
        return app.exec();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
