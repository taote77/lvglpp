#ifndef _LANG_CHANGE_PAGE_H
#define _LANG_CHANGE_PAGE_H

#include "I18nWidgets/InText.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include <memory>

using namespace lvglpp;

class LangChangePage : public ::sys::BaseActivity
{
public:
    explicit LangChangePage();
    ~LangChangePage();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    std::string                        _txt{"Start"};
    std::shared_ptr<widgets::I18nText> _label;
    std::shared_ptr<widgets::LvText>   _label1;

    std::shared_ptr<widgets::RoundedButton> _btn_test;
    std::shared_ptr<widgets::RoundedButton> _btn_simu;
};

#endif // _LANG_CHANGE_PAGE_H