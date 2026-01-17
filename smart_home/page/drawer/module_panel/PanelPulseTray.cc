#include "PanelPulseTray.h"
#include "core/log/log.h"
#include "core/sys/Application.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace lvglpp;
using namespace form::design;

constexpr uint32_t   DIALOG_CONTENT_WIDTH{560};
constexpr uint32_t   DIALOG_CONTENT_HEIGHT{300};
constexpr lv_coord_t DIALOG_CONTENT_RADIUS{20};

ModuleDetailPulseTrayDialog::ModuleDetailPulseTrayDialog(const double progress, const int status) : BaseDialog(true), _change_dir_cb(nullptr), _cancel_cb(nullptr)
{
    _dialog_bg_item = std::make_shared<BaseItem>(this->getRoot());
    _dialog_bg_item->setSize(DIALOG_CONTENT_WIDTH, DIALOG_CONTENT_HEIGHT);
    _dialog_bg_item->setRadius(DIALOG_CONTENT_RADIUS);
    _dialog_bg_item->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));
    _dialog_bg_item->setAligment(LV_ALIGN_TOP_MID, 0, 72);

    _title_label = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24Bold);
    _title_label->setAligment(LV_ALIGN_TOP_LEFT, 24, 20);

    _tray_state_label = std::make_shared<ConsumeText>(lvTr("Status.Abnormal"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);

    std::string status_str;
    if (status == 1)
    {
        status_str = "Status.Normal";
        _tray_state_label->setTextClr(CLR_ON_INFO);
    } else
    {
        status_str = "Status.Abnormal";
        _tray_state_label->setTextClr(CLR_WARNING);
    }
    _tray_state_label->setText(lvTr(status_str.c_str()));
    _tray_state_label->setAligmentTo(*_dialog_bg_item.get(), LV_ALIGN_TOP_RIGHT, -24, 20);

    _progress_label = std::make_shared<ConsumeText>(lvTr("Panel.PulseUsed"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _progress_label->setAligmentTo(*_title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 24);

    _progress_bar = std::make_shared<ProgressBar>(_dialog_bg_item.get(), 115, 20);
    _progress_bar->setAligmentTo(*_progress_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    _progress_bar->updateValue(progress);

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
    ;
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

ModuleDetailPulseTrayDialog::~ModuleDetailPulseTrayDialog()
{}

void ModuleDetailPulseTrayDialog::UpdatePulseTrayInfo(const double progress, const int32_t resin_level, const int32_t tank_type, const int tank_state, const bool sensor_resin,
                                                      const int32_t remain_resin, const int32_t resin_temp)
{
    std::string status_str;
    if (remain_resin < 0)
    {
        status_str = "EMPTY_INFO";
    } else
    {
        status_str = std::to_string(remain_resin) + " g";
    }
    _resin_remain_label->setText(lvTr(status_str.c_str()));
    if (resin_temp < 0)
    {
        _temperature_value_label->setText(lvTr("EMPTY_INFO"));
    } else
    {
        constexpr int ratio_value{10000};
        static auto   doubleToString = [](double value, int precision = 2) -> std::string {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(precision) << value;
            return oss.str();
        };

        double temp = static_cast<double>(resin_temp) / ratio_value;

        //                _resin_temp->setText(doubleToString(temp, 1) + " ℃");

        _temperature_value_label->setText(doubleToString(temp, 1) + " ℃");
    }

    if (tank_type == 1)
    {
        _title_label->setText(lvTr("RegularResinTank"));
    } else if (tank_type == 2)
    {
        _title_label->setText(lvTr("PulsingReleaseResinTank"));
    } else
    {
        _title_label->setText(lvTr("EMPTY_INFO"));
    }

    LOG_DEBUG() << "progress: " << progress;
    if (progress < 75.0F)
    {
        _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));
        _warn_lbl->setVisible(false);

        _warn_bar_0->setVisible(false);
    } else if (progress < 100.0F)
    {
        _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_WARN_FILL));

        _warn_lbl->setTextClr(CLR_PROGRESS_WARN_FILL);
        _warn_lbl->setText(lvTr("WearingOut"));

        _warn_bar_bg->setVisible(true);
        _warn_bar_bg->setBgColor(CLR_WARNING_CONTAINER);
    } else
    {
        _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_ERROR_FILL));

        _warn_lbl->setTextClr(CLR_PROGRESS_ERROR_FILL);
        _warn_lbl->setText(lvTr("PleaseReplace"));

        //            _status_label_data->setTextClr(CLR_WARNING);
        //            _status_label_data->setText(lvTr("EMPTY_INFO"));

        _warn_bar_bg->setVisible(true);

        _warn_bar_bg->setBgColor(CLR_ERROR_CONTAINER);

        //  即将耗尽
    }

    LOG_DEBUG() << "resin_level:" << resin_level;
    _progress_bar->updateValue(progress);

    bool progressbar_visual{true};

    std::string status_top_str;
    if (tank_state == 1)
    {
        status_top_str = "Status.Normal";
        _tray_state_label->setTextClr(CLR_ON_SURFACE);
    } else if (tank_state == 0)
    {
        status_top_str = "Status.Abnormal";
        _tray_state_label->setTextClr(CLR_WARNING);

        progressbar_visual = false;
    } else if (tank_state == -1)
    {
        status_top_str = "Status.Error";
        _tray_state_label->setTextClr(CLR_ERROR);

        progressbar_visual = false;
    }

    _progress_bar->setVisible(progressbar_visual);
    _progress_bar_replace->setVisible(!progressbar_visual);

    _tray_state_label->setText(lvTr(status_top_str.c_str()));
    _tray_state_label->setAligmentTo(*_dialog_bg_item.get(), LV_ALIGN_TOP_RIGHT, -24, 20);
}

void ModuleDetailPulseTrayDialog::open()
{
    BaseDialog::open();
}

void ModuleDetailPulseTrayDialog::close()
{
    BaseDialog::close();
}

void PanelPulseTray::updateTrayInfo(const int32_t peeled_count, const int32_t resin_level, const int32_t tank_type, const int32_t tank_state, const bool sensor_resin,
                                    const int32_t remain_resin, const int32_t resin_temperature)
{
    int32_t warning_loss = 0;
    int32_t danger_loss  = {100000};

    // resin_level 料瓶余量

    if (_last_peeled_count != peeled_count || _last_resin_level != resin_level || _last_tank_type != tank_type || _last_tank_state != tank_state
        || _last_sensor_resin != sensor_resin || _last_resin_remain != remain_resin || _last_resin_temperature != resin_temperature)
    {
        if (tank_state == 0)
        {
            _progress->setVisible(false);

            _label_mod_status->setText(lvTr("Status.Uninstalled"));
            _label_mod_status->setTextClr(CLR_WARNING);
            _label_mod_status->setVisible(true);

            _label_tray_text->setText(lvTr("EMPTY_INFO"));
        } else
        {
            if ((tank_type == 1 || tank_type == 3))
            {
                warning_loss = 75000;
                danger_loss  = 100000;
                _label_tray_text->setText(lvTr("RegularResinTank"));
            } else if (tank_type == 2)
            {
                warning_loss = 112500;
                danger_loss  = 150000;
                _label_tray_text->setText(lvTr("PulsingReleaseResinTank"));
            }

            _progress->setVisible(true);
            _label_mod_status->setVisible(false);
        }

        double percent{0.0F};

        percent = (static_cast<double>(peeled_count)) / (static_cast<double>(danger_loss)) * 100;
        LOG_DEBUG() << "tray update progress: " << percent << "pc: " << peeled_count << "wl: " << warning_loss << "dl: " << danger_loss;

        if (percent < 75.0F)
        {
            _progress->setIndicatorColor(CLR_SECONDARY_DANGER_MASTER);
            _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));
        } else if (percent < 100.0F)
        {
            _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_WARN_FILL));
        } else
        {
            _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_ERROR_FILL));
        }

        auto vall = static_cast<int>(percent);
        vall      = std::clamp(vall, 0, 100);
        _progress->updateValue(vall);

        _last_peeled_count = peeled_count;

        LOG_DEBUG() << "tray info: " << peeled_count << "  " << resin_level << "  " << tank_type << "  " << tank_state;

        _last_peeled_count      = peeled_count;
        _last_resin_level       = resin_level;
        _last_tank_type         = tank_type;
        _last_tank_state        = tank_state;
        _lastest_percent        = percent;
        _last_sensor_resin      = sensor_resin;
        _last_resin_remain      = remain_resin;
        _last_resin_temperature = resin_temperature;

        if (_pulse_tray_detail_dialog)
        {
            _pulse_tray_detail_dialog->UpdatePulseTrayInfo(_lastest_percent, _last_resin_level, _last_tank_type, _last_tank_state, _last_sensor_resin, _last_resin_remain,
                                                           _last_resin_temperature);
        }
    }
}

PanelPulseTray::PanelPulseTray(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(331, 115);
    this->setOpacity(1);
    this->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER_HIGHEST));
    this->setRadius(8);

    constexpr short offset{40};
    constexpr short mod_item_edge_x{16};
    constexpr short mod_item_edge_row2_y{60};

    _icon_tray = std::make_shared<widgets::Image>(":/image/36x36/mod_tray.png", this);
    _icon_tray->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(18));

    if (sys::Application::getLanguageType() == sys::Application::LanguageType::English)
    {
        _label_tray_text = std::make_shared<ConsumeText>(lvTr("PulsingReleaseResinTank"), CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
        _label_tray_text->setPos(lv_coord_t(mod_item_edge_x + offset), lv_coord_t(8));
    } else
    {
        _label_tray_text = std::make_shared<ConsumeText>(lvTr("PulsingReleaseResinTank"), CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
        _label_tray_text->setPos(lv_coord_t(mod_item_edge_x + offset), lv_coord_t(18));
    }

    constexpr int panel_text_max_length{260};
    _label_tray_text->setWidth(panel_text_max_length);

    _label_mod_status = std::make_shared<ConsumeText>(lvTr("Status.Uninstalled"), CLR_ON_SURFACE, this, ConsumeText::FontSize20);
    _label_mod_status->setReColorEnable(true);
    _label_mod_status->setTextClr(CLR_WARNING);
    _label_mod_status->setPos(mod_item_edge_x, 60);
    _label_mod_status->setVisible(true);

    _touch_area = std::make_shared<widgets::MouseArea>(this);
    _touch_area->setSize(150, 92);
    _touch_area->setAligment(LV_ALIGN_RIGHT_MID, 0, 0);

    _touch_area->setOnClickedListener([this]() {
        LOG_DEBUG() << "  pulse tray detail";

        if (!_pulse_tray_detail_dialog)
        {
            _pulse_tray_detail_dialog = std::make_shared<ModuleDetailPulseTrayDialog>(_last_peeled_count, _last_tank_type);
            // _pulse_tray_detail_dialog->setAutoDel(false);
        }

        _pulse_tray_detail_dialog->UpdatePulseTrayInfo(_lastest_percent, _last_resin_level, _last_tank_type, _last_tank_state, _last_sensor_resin, _last_resin_remain,
                                                       _last_resin_temperature);
        _pulse_tray_detail_dialog->open();
    });

    _arrow_right = std::make_shared<widgets::Image>(":/image/32x32/arrow_right.png", this);
    _arrow_right->setSize(32, 32);
    _arrow_right->setAligment(LV_ALIGN_BOTTOM_RIGHT, -16, -15);

    static auto createProgressBar = [](BaseItem *parent) -> std::shared_ptr<wrapper::widgets::ProgressBar> {
        auto progress = std::make_shared<wrapper::widgets::ProgressBar>(parent, 120, 20, 0, 100, 0);
        progress->setBgRadius(4);
        progress->setIndicatorRadius(4);
        progress->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(mod_item_edge_row2_y + 10));

        progress->setMainColor(ThemeColor(CLR_SURFACE_CONTAINER_LOWEST));
        progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));

        return progress;
    };

    _progress = createProgressBar(this);
    _progress->setVisible(false);

    updateTrayInfo(0, 0, 0, 0, false, 0, 0);
}
} // namespace lvglpp::ui::page
