#include "LangChange.h"
#include "LvTranslator.h"
#include "core/sys/Navigators.h"
#include "core/widgets/GlobalVar.h"
#include <core/log/log.h>
#include <iostream>

LangChangePage::LangChangePage() : sys::BaseActivity()
{}

LangChangePage::~LangChangePage()
{}

void LangChangePage::onCreate(void *arg)
{
    _label = std::make_shared<lvglpp::widgets::LvText>(lvglpp::LvTranslator::tr("ChangeLangWarnText"), CLR_SUCCESS_CONTAINER, getRootItem());

    _label->setAligment(LV_ALIGN_CENTER, 0, -120);

    auto btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "中文", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, -50);
    btn->setOnClickedListener([this]() {
        //
        LOG_DEBUG() << "Chinese lang";

        _txt = (_txt == "Start") ? "Stop" : "Start";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::CHINESE);
    });
    _btn_chinese = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "英文", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 0);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";
        //
        LOG_DEBUG() << "English lang";
        std::cout << "dsdsdsds";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::ENGLISH);
    });
    _btn_english = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "日文", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 50);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Japanese lang";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::JAPANESE);
    });

    _btn_japanese = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "俄语", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 100);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Russian lang";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::RUSSIAN);
    });
    _btn_russian = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "阿拉伯语", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 150);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Arabic lang";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::AR);
    });
    _btn_ar = btn;
}

void LangChangePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void LangChangePage::onDestroy()
{}
