/**************************************************************************

Class:${CLASS}

Description:一个空白页的基础实现界面

**************************************************************************/

#ifndef BASE_ACTIVITY_H
#define BASE_ACTIVITY_H

#include "Activity.h"

namespace heygears {
namespace sys {

class BaseActivity : public Activity
{
public:
    BaseActivity() = default;

    ~BaseActivity() override;

    // 禁用拷贝。类拷贝时会导致window窗口被多次释放
    BaseActivity(const BaseActivity &) = delete;

    BaseActivity &operator=(const BaseActivity &) = delete;

protected:
    /**
     * @brief 首次调用时回调创建界面
     * @param arg
     */
    void onCreate(void *arg) override;

    /**
     * @brief 开始显示前调用
     */
    void onResume() override;

    /**
     * @brief 结束显示时调用
     */
    void onPause() override;

    /**
     * @brief 销毁界面时调用
     */
    void onDestroy() override;

    void onNotifyUI(const Event &e) override;

    /**
     * @brief  返回根屏幕的指针
     * @return
     */
    widgets::BaseItem *getRootWindow() const override;

private:
    widgets::BaseItem *window_ptr{};
};
} // namespace sys
} // namespace heygears

#endif // BASE_ACTIVITY_H
