#include "HomePage.h"
#include "core/log/log.h"
#include "core/sys/Activity.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Toast.h"
#include "page/sensor/SensorActivity.h"
#include "theme/form_design.h"
#include <format>
#include <i18n/LvTranslator.h>

using namespace lvglpp;
using namespace form::design;

HomePage::HomePage() : sys::BaseActivity()
{}

HomePage::~HomePage()
{}

void HomePage::onCreate(std::any arg)
{
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(form::design::CLR_SURFACE);

    _bg_img = std::make_unique<widgets::Image>(":/image/800x480/home_card.png", getRoot());
    _bg_img->setAligment(LV_ALIGN_CENTER, 0, 0);

    _label = std::make_unique<LvText>(lvTr("Home Page"), ThemeColor(CLR_ON_SURFACE), getRoot(), LvText::FontSize24Bold);
    _label->setAligment(LV_ALIGN_CENTER, 0, -40);

    // RoundedButton(int width, int height, ColorStyle type, const std::string &strMsg, BaseItem *parent);

    _toast_btn = std::make_unique<RoundedButton>(140, 40, RoundedButton::ColorStyle::Blue, lvTr("Toast"), getRoot());
    _toast_btn->setAligment(LV_ALIGN_CENTER, 0, 40);
    _toast_btn->setOnClickedListener([this]() {
        // sys::StackView::getInstance().push(std::make_shared<ToastActivity>());
        widgets::Toast::success(lvTr("Click Success!"));
    });

    _drawer = std::make_shared<lvglpp::ui::page::TopDrawer>(this->getRoot());

    _timer = lv_timer_create(
        [](lv_timer_t *t) {
            auto *self = static_cast<HomePage *>(lv_timer_get_user_data(t));
            static int    count       = 0;
            constexpr int fixed_times = 15;
            if (count >= fixed_times) {
                lv_timer_del(t);
                self->_timer = nullptr;
                return;
            }

            // widgets::Toast::success(msg);
            LOG_INFO() << "HomePage::_timer callback" << count;
            count++;
        },
        1500, this);
}

void HomePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
    // sys::Activity::onNotifyUI(evt);
}

void HomePage::onDestroy()
{
    if (_timer) {
        lv_timer_del(_timer);
        _timer = nullptr;
    }
}
