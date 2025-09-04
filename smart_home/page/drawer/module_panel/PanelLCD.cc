#include "PanelLCD.h"
#include "core/log/log.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <algorithm>

namespace lvglpp::ui::page {
using namespace lvglpp;
using namespace form::design;
using namespace form;

constexpr uint32_t   DIALOG_CONTENT_WIDTH{728};
constexpr uint32_t   DIALOG_CONTENT_HEIGHT{464};
constexpr lv_coord_t DIALOG_CONTENT_RADIUS{20};

LCDInfoCard::LCDInfoCard(const double expose_time, const int32_t status) : BaseDialog(true), _change_dir_cb(nullptr), _cancel_cb(nullptr)
{
    _dialog_bg_item = std::make_shared<BaseItem>(this->getRoot());
    _dialog_bg_item->setSize(DIALOG_CONTENT_WIDTH, DIALOG_CONTENT_HEIGHT);
    _dialog_bg_item->setRadius(DIALOG_CONTENT_RADIUS);
    _dialog_bg_item->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));
    _dialog_bg_item->setAligment(LV_ALIGN_TOP_MID, 0, 28);

    _title_label = std::make_shared<ConsumeText>(lvTr("LCDScreenLable"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24Bold);
    _title_label->setAligment(LV_ALIGN_TOP_LEFT, 24, 20);

    _lcd_state_label = std::make_shared<ConsumeText>(lvTr("Status.Abnormal"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);

    std::string status_str;
    if (status == 1)
    {
        status_str = "Status.Normal";
        _lcd_state_label->setTextClr(CLR_ON_INFO);
    } else
    {
        status_str = "Status.Abnormal";
        _lcd_state_label->setTextClr(CLR_WARNING);
    }
    _lcd_state_label->setText(lvTr(status_str.c_str()));
    _lcd_state_label->setAligmentTo(*_dialog_bg_item.get(), LV_ALIGN_TOP_RIGHT, -24, 20);

    _progress_label = std::make_shared<ConsumeText>(lvTr("Panel.PulseUsed"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _progress_label->setAligmentTo(*_title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 24);

    _progress_bar = std::make_shared<ProgressBar>(_dialog_bg_item.get(), 115, 20);
    _progress_bar->setAligmentTo(*_progress_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    // _progress_bar->updateValue(0);

    _progress_bar->setBgRadius(4);
    _progress_bar->setIndicatorRadius(4);

    _progress_bar->setMainColor(ThemeColor(CLR_SURFACE_CONTAINER_LOWEST));
    _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));
    _progress_bar->setVisible(false);

    _progress_bar_replace = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _progress_bar_replace->setAligmentTo(*_progress_label, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

    _warn_lbl = std::make_shared<ConsumeText>("", ThemeColor(CLR_WARNING), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _warn_lbl->setAligmentTo(*_progress_bar, LV_ALIGN_OUT_RIGHT_MID, 12, 0);
    _warn_lbl->setWidth(120);

    /// -------------

    _status_label = std::make_shared<ConsumeText>(lvTr("Panel.Resin.Remaining"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize22);
    _status_label->setPos(24, 82 + 32 + 15);

    _resin_remain_label = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize22);
    _resin_remain_label->setAligmentTo(*_status_label, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

    _temperature_tag_label = std::make_shared<ConsumeText>(lvTr(""), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize22);
    _temperature_tag_label->setPos(286, 82 + 32 + 15);

    _temperature_value_label = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize22);

    _temperature_value_label->setAligmentTo(*_temperature_tag_label, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

    _warn_bar_bg = std::make_shared<BaseItem>(this->getRoot());
    _warn_bar_bg->setSize(512, 48);
    _warn_bar_bg->setRadius(RADIUS_12);
    _warn_bar_bg->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));
    _warn_bar_bg->setAligment(LV_ALIGN_TOP_MID, 0, 72);
    _warn_bar_bg->setAligmentTo(*_status_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 16);
    _warn_bar_bg->setVisible(false);

    _warn_icon = std::make_shared<widgets::Image>(":/image/28x28/warn_icon.png", _warn_bar_bg.get());
    _warn_icon->setAligment(LV_ALIGN_LEFT_MID, 12, 0);

    _warn_bar_0 = std::make_shared<ConsumeText>(lvTr("ReplaceMaterialTrayTips"), ThemeColor(CLR_ON_SURFACE), _warn_bar_bg.get(), ConsumeText::FontSize24);
    // _warn_bar_0 = std::make_shared<ConsumeText>("请尽快更换料盘。", ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _warn_bar_0->setAligmentTo(*_warn_icon, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

    _warn_bar_1 = std::make_shared<ConsumeText>("", ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _warn_bar_1->setAligmentTo(*_warn_bar_0, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    _close_button = std::make_shared<RoundedButton>(48, 48, RoundedButton::ColorStyle::Gray, "", ":/image/32x32/x_close.png", _dialog_bg_item.get());
    _close_button->setRadius(24);
    _close_button->setAligment(LV_ALIGN_BOTTOM_MID, 0, -20);
    _close_button->setOnClickedListener([this]() {
        LOG_DEBUG() << "close mod detail dialog";
        this->close();
    });
}

LCDInfoCard::~LCDInfoCard()
{}

void LCDInfoCard::UpdateLCDInfo(const double expose_time, const int32_t status)
{}

void LCDInfoCard::open()
{
    BaseDialog::open();
}

void LCDInfoCard::close()
{
    BaseDialog::close();
}

void PanelLCD::updateScreenInfo(const double progress)
{
    // constexpr double min_step{0.00001F};
    if (_last_progress != progress)
    {
        LOG_DEBUG() << "updateScreenInfo: " << progress;

        int ratio = static_cast<int>(progress) * 100;
        std::clamp(ratio, 0, 100);
        _progress->updateValue(ratio);
        _last_progress = progress;

        if (ratio < 75)
        {
            _progress->setIndicatorColor(CLR_SECONDARY_DANGER_MASTER);
            _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));
        } else if (ratio < 100)
        {
            _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_WARN_FILL));
        } else
        {
            _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_ERROR_FILL));
        }
    }
}

PanelLCD::PanelLCD(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(331, 115);
    this->setOpacity(1);
    this->setBgColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER_HIGHEST));
    this->setRadius(8);

    constexpr short offset{40};
    constexpr short mod_item_edge_x{16};
    constexpr short mod_item_edge_row2_y{60};

    _text  = std::make_shared<ConsumeText>("", CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
    _text1 = std::make_shared<ConsumeText>("", CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
    _text->setPos(mod_item_edge_x + offset, 16);
    _text1->setPos(mod_item_edge_x + offset, 60);

    _touch_area = std::make_shared<widgets::MouseArea>(this);
    _touch_area->setSize(150, 92);
    _touch_area->setAligment(LV_ALIGN_RIGHT_MID, 0, 0);

    _arrow_right = std::make_shared<widgets::Image>(":/image/32x32/arrow_right.png", this);
    _arrow_right->setSize(32, 32);
    _arrow_right->setAligment(LV_ALIGN_BOTTOM_RIGHT, -16, -15);

    _touch_area->setOnClickedListener([this]() {
        LOG_DEBUG() << "lcd info card";

        // TODO
        return;
        if (!_info_card)
        {
            _info_card = std::make_shared<LCDInfoCard>(2, 2);

            _info_card->setAutoDel(false);
        }

        _info_card->UpdateLCDInfo(2, 2);
        _info_card->open();
    });

    static auto createProgressBar = [](BaseItem *parent) -> std::shared_ptr<wrapper::widgets::ProgressBar> {
        auto progress = std::make_shared<wrapper::widgets::ProgressBar>(parent, 120, 20, 0, 100, 0);
        progress->setBgRadius(4);
        progress->setIndicatorRadius(4);
        progress->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(mod_item_edge_row2_y + 10));

        progress->setMainColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER_LOWEST));
        progress->setIndicatorColor(design::ThemeColor(design::CLR_PROGRESS_NORMAL_FILL));

        return progress;
    };

    _icon1 = std::make_shared<widgets::Image>(":/image/36x36/mod_lcd.png", this);
    _icon1->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(18));

    _text->setPos(lv_coord_t(16 + offset), lv_coord_t(18));
    _text->setText(lvTr("LCDScreenLable"));

    _progress = createProgressBar(this);

    updateScreenInfo(0);
}
} // namespace lvglpp::ui::page