#ifndef SNAP_SHOT_H
#define SNAP_SHOT_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include <memory>

using namespace lvglpp;

class SnapShot : public ::sys::BaseActivity
{
public:
    explicit SnapShot();
    ~SnapShot();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

    void takeShot();

private:
    std::string                             _txt{"Start"};
    std::shared_ptr<widgets::LvText>        _label;
    std::shared_ptr<widgets::RoundedButton> _btn;
};

#endif // SNAP_SHOT_H