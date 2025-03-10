#ifndef _HOME_H
#define _HOME_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/ConsumeText.h"
#include "core/sys/Event.h"
#include <memory>

using namespace heygears;
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
    std::shared_ptr<widgets::ConsumeText> _label;
};

#endif