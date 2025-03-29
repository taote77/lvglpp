
#include "BaseActivity.h"
#include "Navigators.h"
#include <lvgl.h>

namespace lvglpp {
namespace sys {
BaseActivity::~BaseActivity() noexcept
{
    if (_root_win != nullptr)
    {
        delete _root_win;
        _root_win = nullptr;
    }
}

void BaseActivity::onCreate(void *arg)
{
    LOG_DEBUG() << "BaseActivity onCreate,class name:" << typeid(*this).name();
    // 获取当前活动的窗口
    _root_win = new widgets::BaseItem();
    _root_win->setSize(LV_HOR_RES_MAX, LV_VER_RES_MAX);
}

void BaseActivity::onResume()
{
    LOG_DEBUG() << "BaseActivity onResume,class name:" << typeid(*this).name();
    if (_root_win != nullptr)
    {
        LOG_DEBUG() << "onResume: window_ptr != nullptr";
        _root_win->setVisible(true);
    }
}

void BaseActivity::onPause()
{
    // hDebug() << "BaseActivity onPause,class name:" << typeid(*this).name();
    if (_root_win != nullptr)
    {
        _root_win->setVisible(false);
    }
}

void BaseActivity::onDestroy()
{
    LOG_DEBUG() << "BaseActivity onDestroy,class name:" << typeid(*this).name();
    if (_root_win != nullptr)
    {
        LOG_DEBUG() << "onDestroy: _root_win != nullptr";
        delete _root_win;
        _root_win = nullptr;
    }
}

widgets::BaseItem *BaseActivity::getRootItem() const
{
    return _root_win;
}

void BaseActivity::onNotifyUI(const Event &e)
{
    // LV_LOG_USER("notify index:%d",(int)et);
}
} // namespace sys
} // namespace lvglpp