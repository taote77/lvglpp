#ifndef _LANG_CHANGE_PAGE_H
#define _LANG_CHANGE_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include <memory>

using namespace lvglpp;

class RemoteClient : public ::sys::BaseActivity
{
public:
    explicit RemoteClient();
    ~RemoteClient();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    lv_timer_t *_timer;

    std::shared_ptr<widgets::LvText> _label;

    std::shared_ptr<widgets::RoundedButton> _btn_test;
    std::shared_ptr<widgets::RoundedButton> _btn_simu;

    int _count{0};
};

#endif // _LANG_CHANGE_PAGE_H