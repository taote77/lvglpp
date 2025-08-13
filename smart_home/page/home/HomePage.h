#ifndef _HOME_PAGE_H
#define _HOME_PAGE_H

#include "Drawer.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/Text.h"
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
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

private:
    std::shared_ptr<lvglpp::ui::page::Drawer> _drawer;

    std::string                             _txt{"Start"};
    std::shared_ptr<widgets::LvText>        _label;
    std::shared_ptr<widgets::GifImage>      _gif;
    std::shared_ptr<widgets::RoundedButton> _btn;

    std::shared_ptr<widgets::RoundedButton> _btn_air_temp;
};

#endif // _HOME_PAGE_H
