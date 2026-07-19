#ifndef _BIND_DEMO_H
#define _BIND_DEMO_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/Text.h"
#include <core/widgets/GifImage.h>
#include <core/widgets/Image.h>
#include <core/widgets/LottieCanvas.h>

#include <memory>

using namespace lvglpp;

class BindDemo : public ::sys::BaseActivity
{
public:
    explicit BindDemo();
    ~BindDemo();

protected:
    void onCreate(std::any arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    std::string                      _txt{"Start"};
    std::shared_ptr<widgets::LvText> _label;

    std::shared_ptr<widgets::Image>        _img;
    std::shared_ptr<widgets::GifImage>     _gif;
    std::shared_ptr<widgets::LottieCanvas> _lottie;

    std::shared_ptr<widgets::RoundedButton> _btn;
};

#endif // _BIND_DEMO_H