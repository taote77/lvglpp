#include "BaseActivity.h"
#include "InputManager.h"
#include "StackView.h"
#include "UiConfig.h"
#include <lvgl.h>

namespace lvglpp::sys {

BaseActivity::~BaseActivity() noexcept
{
    if (_focus_scope != 0)
    {
        InputManager::getInstance().popScope(_focus_scope);
        _focus_scope = 0;
    }
    _root_win.reset();
}

void BaseActivity::onCreate(std::any arg)
{
    LOG_DEBUG() << "BaseActivity onCreate,class name:" << typeid(*this).name();
    _root_win = std::make_unique<widgets::BaseItem>();
    _root_win->setSize(UiConfig::instance().width(), UiConfig::instance().height());
}

void BaseActivity::onConfigurationChanged(const UiConfiguration &configuration)
{
    if (_root_win)
    {
        _root_win->setSize(configuration.width, configuration.height);
    }
    refreshFocus();
}

void BaseActivity::onResume()
{
    LOG_DEBUG() << "BaseActivity onResume,class name:" << typeid(*this).name();
    if (_root_win)
    {
        LOG_DEBUG() << "onResume: window_ptr != nullptr";
        _root_win->setVisible(true);

        if (_focus_scope == 0)
        {
            _focus_scope = InputManager::getInstance().pushScope(_root_win.get());
        } else
        {
            InputManager::getInstance().refresh(_focus_scope);
        }
    }
}

void BaseActivity::onPause()
{
    if (_focus_scope != 0)
    {
        InputManager::getInstance().popScope(_focus_scope);
        _focus_scope = 0;
    }

    if (_root_win)
    {
        _root_win->setVisible(false);
    }
}

void BaseActivity::onDestroy()
{
    LOG_DEBUG() << "BaseActivity onDestroy,class name:" << typeid(*this).name();
    if (_focus_scope != 0)
    {
        InputManager::getInstance().popScope(_focus_scope);
        _focus_scope = 0;
    }
    _root_win.reset();
}

widgets::BaseItem *BaseActivity::getRoot() const
{
    return _root_win.get();
}

void BaseActivity::refreshFocus()
{
    if (_focus_scope != 0)
    {
        InputManager::getInstance().refresh(_focus_scope);
    }
}

void BaseActivity::onNotifyUI(const Event &e)
{}

} // namespace lvglpp::sys
