#ifndef _HOME_PAGE_H
#define _HOME_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
#include "page/drawer/TopDrawer.h"
#include <core/widgets/GifImage.h>
#include <core/widgets/Image.h>
#include <memory>

#include "qgl/timer/Timer.h"

using namespace lvglpp;

class HomePage : public ::sys::BaseActivity
{
public:
    explicit HomePage();

    ~HomePage();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    std::shared_ptr<lvglpp::ui::page::TopDrawer> _drawer;

    std::unique_ptr<widgets::Image> _bg_img;

    std::string                        _txt{"Start"};
    std::unique_ptr<widgets::LvText>   _label;
    std::unique_ptr<widgets::GifImage> _gif;

    std::unique_ptr<QGL::Timer> _timer; // Changed from std::unique_ptr<QGL::Timer> to std::unique_ptr<qgl::Timer>
    
    
    std::unique_ptr<widgets::RoundedButton> _toast_btn;
};

#endif // _HOME_PAGE_H
