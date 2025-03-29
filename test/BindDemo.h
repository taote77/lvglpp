#ifndef _BIND_DEMO_H
#define _BIND_DEMO_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include <memory>

using namespace lvglpp;

class BindDemo : public ::sys::BaseActivity
{
public:
    explicit BindDemo();
    ~BindDemo();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    std::string                             _txt{"Start"};
    std::shared_ptr<widgets::LvText>        _label;
    std::shared_ptr<widgets::RoundedButton> _btn;
};

#endif // _BIND_DEMO_H