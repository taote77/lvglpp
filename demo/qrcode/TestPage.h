#ifndef TEST_PAGE_SIG_SLOT_H
#define TEST_PAGE_SIG_SLOT_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include <core/widgets/QrCode.h>
#include <memory>

using namespace lvglpp;

class TestPage : public ::sys::BaseActivity
{
public:
    explicit TestPage();
    ~TestPage();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    std::shared_ptr<widgets::QrCode> _qr_code;
};

#endif // TEST_PAGE_SIG_SLOT_H