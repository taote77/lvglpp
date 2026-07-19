#ifndef _HOME_PAGE_H
#define _HOME_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/Text.h"
#include "page/drawer/TopDrawer.h"
#include <core/widgets/GifImage.h>
#include <core/widgets/Image.h>
#include <memory>

using namespace lvglpp;

class HomePage : public ::sys::BaseActivity
{
public:
    explicit HomePage();

    ~HomePage();

protected:
    void onCreate(std::any arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    std::shared_ptr<lvglpp::ui::page::TopDrawer> _drawer;

    std::unique_ptr<widgets::Image> _bg_img;

    std::string                        _txt{"Start"};
    std::unique_ptr<widgets::LvText>   _label;
    std::unique_ptr<widgets::GifImage> _gif;

    lv_timer_t *_timer = nullptr;
    
    
    std::unique_ptr<widgets::RoundedButton> _toast_btn;
};

#endif // _HOME_PAGE_H
