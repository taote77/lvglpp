#include "PanelPulseMod.h"
#include "core/log/log.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {
using namespace lvglpp;
using namespace form::design;

void PanelPusleMod::updatePulseFilmLife(double inflator_worktime, int32_t tanktype, int32_t airmod_state)
{
    bool trigger_ui{false};

    if ((_last_tanktype != tanktype) || (_last_airmod_state != airmod_state) || (_last_usesdtime != inflator_worktime))
    {
        if (airmod_state == 0)
        {
            _progress->setVisible(false);

            _label_status->setText(lvTr("Status.Uninstalled"));
            _label_status->setTextClr(CLR_WARNING);
            _label_status->setVisible(true);
        } else
        {
            if (inflator_worktime < 2500)
            {
                _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));
            } else if (inflator_worktime > 3000)
            {
                _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_WARN_FILL));
            } else
            {
                _progress->setIndicatorColor(ThemeColor(CLR_PROGRESS_ERROR_FILL));
            }

            double percent = inflator_worktime / 3000.0;
            _progress->updateValue(percent);
            _progress->setVisible(true);
            _label_status->setVisible(false);
        }

        _last_tanktype     = tanktype;
        _last_airmod_state = airmod_state;
        _last_usesdtime    = inflator_worktime;

        LOG_DEBUG() << "inflator_worktime " << inflator_worktime << "tanktype " << tanktype << "airmod_state " << airmod_state;

        if (_pulse_detail_dialog)
        {
            _pulse_detail_dialog->updatePulseFilmInfo(_last_usesdtime, _last_tanktype, _last_airmod_state);
        }
    }
}

PanelPusleMod::PanelPusleMod(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(331, 115);
    this->setOpacity(1);
    this->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER_HIGHEST));
    this->setRadius(8);

    constexpr short offset{40};
    constexpr short mod_item_edge_x{16};
    constexpr short mod_item_edge_row2_y{60};

    _icon1_mod_pulse = std::make_shared<widgets::Image>(":/image/36x36/mod_pulse.png", this);
    _icon1_mod_pulse->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(18));

    _label_mod_pulse = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
    _label_mod_pulse->setPos(lv_coord_t(mod_item_edge_x + offset), lv_coord_t(18));
    // _label_mod_pulse->setAligment(LV_ALIGN_BOTTOM_LEFT, 16, -12);

    _label_mod_pulse->setText(lvTr("ElectronStack"));
    // _label_mod_pulse->setLongMode(1);
    constexpr int panel_text_max_length{270};
    _label_mod_pulse->setLongMode(panel_text_max_length, ConsumeText::LongMode::MiddleEllipsis);

    _label_status = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), CLR_ON_SURFACE, this, ConsumeText::FontSize20);
    _label_status->setPos(mod_item_edge_x, 60);

    _touch_area = std::make_shared<widgets::MouseArea>(this);
    _touch_area->setSize(150, 92);
    _touch_area->setAligment(LV_ALIGN_RIGHT_MID, 0, 0);

    _touch_area->setOnClickedListener([this]() {
        LOG_DEBUG() << "  pulse mod detail";

        if (!_pulse_detail_dialog)
        {
            _pulse_detail_dialog = std::make_shared<ModuleDetailPulseDialog>(_last_usesdtime, _last_airmod_state);

            // _pulse_detail_dialog->setAutoDel(false);
        }

        _pulse_detail_dialog->updatePulseFilmInfo(_last_usesdtime, _last_tanktype, _last_airmod_state);
        _pulse_detail_dialog->open();
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

    updatePulseFilmLife(_last_usesdtime, _last_tanktype, _last_airmod_state);
}
} // namespace lvglpp::ui::page