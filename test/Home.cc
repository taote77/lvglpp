#include "Home.h"

#include "core/widgets/GlobalVar.h"

Home::Home() : sys::BaseActivity() { }

Home::~Home() { }

void Home::onCreate(void *arg)
{

    _label = std::make_shared<widgets::ConsumeText>("Network Preparing...", CLR_PRIMARY_BLACK,
                                                    getRootWindow(),
                                                    widgets::ConsumeText::HeadingBold);

    // _label.reset(new widgets::ConsumeText("Network Preparing...", CLR_PRIMARY_BLACK,
    //                                       getRootWindow(), widgets::ConsumeText::HeadingBold));

    _label->setTextAlign(LV_TEXT_ALIGN_CENTER);
    _label->setAligment(LV_ALIGN_TOP_MID, 0, 424);
}

void Home::onNotifyUI(const sys::Event &evt) { }

void Home::onDestroy() { }
