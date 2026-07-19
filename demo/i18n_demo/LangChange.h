#ifndef _LANG_CHANGE_PAGE_H
#define _LANG_CHANGE_PAGE_H

#include "I18nWidgets/InText.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
// #include "core/widgets/PushButton.h"
#include "core/widgets/Text.h"
#include <memory>

using namespace lvglpp;

class LangChangePage : public ::sys::BaseActivity
{
public:
    explicit LangChangePage();
    ~LangChangePage();

protected:
    void onCreate(std::any arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    std::string                      _txt{"Start"};
    std::unique_ptr<widgets::LvText> _label;
    std::unique_ptr<widgets::LvText> _label1;

    std::unique_ptr<widgets::PushButton> _btn_chinese;
    std::unique_ptr<widgets::PushButton> _btn_english;
    std::unique_ptr<widgets::PushButton> _btn_japanese;
    std::unique_ptr<widgets::PushButton> _btn_russian;
    std::unique_ptr<widgets::PushButton> _btn_test;
    std::unique_ptr<widgets::PushButton> _btn_simu;
};

#endif // _LANG_CHANGE_PAGE_H