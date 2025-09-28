#include "HomePage.h"
#include "core/log/log.h"
#include "core/sys/Activity.h"
#include "core/sys/TaskStack.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Toast.h"
#include "page/sensor/SensorActivity.h"
#include "theme/form_design.h"
#include <i18n/LvTranslator.h>

#include "qgl/signals/Signal.h"

using namespace lvglpp;
using namespace form::design;

HomePage::HomePage() : sys::BaseActivity()
{}

HomePage::~HomePage()
{}

void HomePage::onCreate(void *arg)
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
        // sys::TaskStack::getInstance()->push(std::make_shared<ToastActivity>());
        widgets::Toast::success(lvTr("Click Success!"));
    });

    _drawer = std::make_shared<lvglpp::ui::page::TopDrawer>(this->getRoot());


    //  Timer(int interval = 1000, TimerType type = TimerType::Repeating, Object* parent = nullptr);
    // Timer(std::function<void()> callback, int interval = 1000, TimerType type = TimerType::Repeating, Object* parent = nullptr);

    _timer = std::make_unique<QGL::Timer>(
        [this]() -> void {
            // _drawer->updateDeviceInfo(nullptr);

            static int count = 0;
            if (count >= 15)
            {
                _timer->stop();
                // print stop info
                LOG_INFO() << "HomePage::_timer stop";
            }

            LOG_INFO() << "HomePage::_timer callback" << count;
            count++;
        },
        1000, QGL::TimerType::Repeating, nullptr);

        // 1000, QGL::TimerType::SingleShot, nullptr);
    
    
    // CONNECT(_timer.get(), &QGL::Timer::timeout, this, [this]() {
    //     LOG_INFO() << "HomePage::_timer callback";
    //     });
    
    _timer->start();
    
}

void HomePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
    // sys::Activity::onNotifyUI(evt);
}

void HomePage::onDestroy()
{}
