#ifndef _HOME_H
#define _HOME_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/sys/Event.h"
#include <memory>

using namespace lvglpp;

class Home : public ::sys::BaseActivity
{
public:
    explicit Home();
    ~Home();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    std::shared_ptr<widgets::LvText> _label;
    std::shared_ptr<widgets::RoundedButton> _btn;
};

#endif // _HOME_H