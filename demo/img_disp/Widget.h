#ifndef _BIND_DEMO_H
#define _BIND_DEMO_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include <core/widgets/GifImage.h>
#include <core/widgets/Image.h>
#include <memory>

using namespace lvglpp;

class Widget : public ::sys::BaseActivity
{
public:
    explicit Widget();
    ~Widget();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    lv_subject_t _subject;

    std::shared_ptr<widgets::Image>    _img;
    std::shared_ptr<widgets::GifImage> _gif;
};

#endif // _BIND_DEMO_H