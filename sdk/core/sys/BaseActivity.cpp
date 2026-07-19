#include "BaseActivity.h"
#include "StackView.h"
#include <lvgl.h>

namespace lvglpp::sys {

BaseActivity::~BaseActivity() noexcept
{
    _root_win.reset();
}

void BaseActivity::onCreate(std::any arg)
{
    LOG_DEBUG() << "BaseActivity onCreate,class name:" << typeid(*this).name();
    _root_win = std::make_unique<widgets::BaseItem>();
    _root_win->setSize(LV_HOR_RES_MAX, LV_VER_RES_MAX);
}

void BaseActivity::onResume()
{
    LOG_DEBUG() << "BaseActivity onResume,class name:" << typeid(*this).name();
    if (_root_win)
    {
        LOG_DEBUG() << "onResume: window_ptr != nullptr";
        _root_win->setVisible(true);
    }
}

void BaseActivity::onPause()
{
    if (_root_win)
    {
        _root_win->setVisible(false);
    }
}

void BaseActivity::onDestroy()
{
    LOG_DEBUG() << "BaseActivity onDestroy,class name:" << typeid(*this).name();
    _root_win.reset();
}

widgets::BaseItem *BaseActivity::getRoot() const
{
    return _root_win.get();
}

void BaseActivity::onNotifyUI(const Event &e)
{}

} // namespace lvglpp::sys
