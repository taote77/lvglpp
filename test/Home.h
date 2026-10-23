#ifndef _HOME_H
#define _HOME_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/Label.h"
#include "core/widgets/PushButton.h"
#include <memory>

using namespace lvglpp;

class Home : public ::sys::BaseActivity
{
public:
    explicit Home();
    ~Home();

protected:
    void onCreate(std::any arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    std::shared_ptr<widgets::Label>        _label;
    std::shared_ptr<widgets::PushButton> _btn;
};

#endif // _HOME_H