#include "PanelHeater.h"
#include "core/log/log.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace lvglpp;
using namespace form::design;

void PanelHeater::updateHeatStatus(int32_t scraperstate, int32_t scrapertype, int32_t resin_temp, bool resin_sensor, bool heater_equipped)
{
    if (_last_scraperstate != scraperstate || _last_scrapertype != scrapertype || _last_resin_temp != resin_temp || _resin_sensor != resin_sensor)
    {
        _last_scraperstate    = scraperstate;
        _last_scrapertype     = scrapertype;
        _last_resin_temp      = resin_temp;
        _resin_sensor         = resin_sensor;
        _last_heater_equipped = heater_equipped;

        //
        switch (_last_scrapertype)
        {
        case 1:
            _label_mod_scraper->setText(lvTr("HeatingScraper"));
            break;
        case 2:
            _label_mod_scraper->setText(lvTr("ScraperNormal"));
            break;
        default:
            _label_mod_scraper->setText(lvTr("Scraper.TitleName"));
            break;
        }

        if (scraperstate == 1)
        {
            _label_scraper_state->setText("");
        } else
        {
            _label_scraper_state->setText(lvTr("Net.Disconnected"));
        }

        if (resin_sensor)
        {
            constexpr int ratio_value{10000};

            static const char *RESIN_TEMP_TAG = " #3760EA %1%℃#";
            // resin_temp_label_->setText();

            static auto doubleToString = [](double value, int precision = 2) -> std::string {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(precision) << value;
                return oss.str();
            };

            double temp = static_cast<double>(resin_temp) / ratio_value;

            //                _resin_temp->setText(doubleToString(temp, 1) + " ℃");
        }
        //            else
        //            {
        //                _resin_temp->setText(lvTrStr("EMPTY_INFO") + " ℃");
        //            }

        // if (_detail_dialog)
        // {
        //     _detail_dialog->updateHeaterStatus(_last_scraperstate, _last_scrapertype, _last_resin_temp, _resin_sensor, _last_heater_equipped);
        // }
    }
}

PanelHeater::PanelHeater(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(331, 115);
    this->setOpacity(1);
    this->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER_HIGHEST));
    this->setRadius(8);

    constexpr short offset{40};
    constexpr short mod_item_edge_x{16};
    constexpr short mod_item_edge_row2_y{60};

    _label_mod_scraper = std::make_shared<ConsumeText>(lvTr("HeatingScraper"), CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
    _label_mod_scraper->setPos(lv_coord_t(mod_item_edge_x + offset), lv_coord_t(18));

    _label_scraper_state = std::make_shared<ConsumeText>("", CLR_WARNING, this, ConsumeText::FontSize22);
    _label_scraper_state->setAligment(LV_ALIGN_BOTTOM_LEFT, 16, -21);

    _touch_area = std::make_shared<widgets::MouseArea>(this);
    _touch_area->setSize(150, 92);
    _touch_area->setAligment(LV_ALIGN_RIGHT_MID, 0, 0);

    _arrow_right = std::make_shared<widgets::Image>(":/image/32x32/arrow_right.png", this);
    _arrow_right->setSize(32, 32);
    _arrow_right->setAligment(LV_ALIGN_BOTTOM_RIGHT, -16, -15);

    _icon1 = std::make_shared<widgets::Image>(":/image/36x36/scraper.png", this);
    _icon1->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(18));

    _touch_area->setOnClickedListener([this]() {
        LOG_DEBUG() << "  heat mod detail" << switch_enable;

        // if (!_detail_dialog)
        // {
        //     _detail_dialog = std::make_shared<ModuleDetailHeaterDialog>(switch_enable);
        //     _detail_dialog->setAutoDel(false);
        // }

        // _detail_dialog->updateHeaterStatus(_last_scraperstate, _last_scrapertype, _last_resin_temp, _resin_sensor, _last_heater_equipped);

        // _detail_dialog->setSwitchStatus(switch_enable);

        // _detail_dialog->open();
    });

    updateHeatStatus(_last_scraperstate, _last_scrapertype, _last_resin_temp, _resin_sensor, _last_heater_equipped);
}
} // namespace lvglpp::ui::page