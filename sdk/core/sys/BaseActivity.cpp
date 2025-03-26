
#include "BaseActivity.h"
#include <lvgl.h>
#include "Navigators.h"

namespace lvglpp {
namespace sys {
BaseActivity::~BaseActivity() noexcept
{
    if (window_ptr != nullptr) {
        delete window_ptr;
        window_ptr = nullptr;
    }
}

void BaseActivity::onCreate(void *arg)
{
    LOG_DEBUG() << "BaseActivity onCreate,class name:" << typeid(*this).name();
    // 获取当前活动的窗口
    window_ptr = new widgets::BaseItem();
    window_ptr->setSize(LV_HOR_RES_MAX, LV_VER_RES_MAX);
}

void BaseActivity::onResume()
{
    LOG_DEBUG() << "BaseActivity onResume,class name:" << typeid(*this).name();
    if (window_ptr != nullptr) {
        LOG_DEBUG() << "onResume: window_ptr != nullptr";
        window_ptr->setVisible(true);
    }
}

void BaseActivity::onPause()
{
    // hDebug() << "BaseActivity onPause,class name:" << typeid(*this).name();
    if (window_ptr != nullptr) {
        window_ptr->setVisible(false);
    }
}

void BaseActivity::onDestroy()
{
    LOG_DEBUG() << "BaseActivity onDestroy,class name:" << typeid(*this).name();
    if (window_ptr != nullptr) {
        LOG_DEBUG() << "onDestroy: window_ptr != nullptr";
        delete window_ptr;
        window_ptr = nullptr;
    }
}

widgets::BaseItem *BaseActivity::getRootWindow() const
{
    return window_ptr;
}

void BaseActivity::onNotifyUI(const Event &e)
{
    // LV_LOG_USER("notify index:%d",(int)et);
}
} // namespace sys
} // namespace lvglpp