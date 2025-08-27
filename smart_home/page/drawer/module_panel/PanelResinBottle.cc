#include "PanelResinBottle.h"
#include "core/log/log.h"
#include "core/sys/Application.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {
using namespace form;
using namespace lvglpp;
using namespace form::design;

void PanelResinBottle::SwitchState(bool state, bool equipped)
{
    _switch_enable  = state;
    _fill_mod_equip = equipped;

    if (_resin_detail_dialog)
    {
        _resin_detail_dialog->setSwitchStatus(state, equipped);
    }
}

void PanelResinBottle::updateBottleInfo(const std::string &material, const int32_t tank_size, const int32_t remaining, const int32_t bottlestate,
                                        const int32_t printerthsensorstate, bool fill_equipped)
{
    // 1-materialname, 2-volume, 3-bottlestate
    if ((_last_material != material) || (_last_remaining != remaining) || (_last_bottlestate != bottlestate) || (_last_printerthsensorstate != printerthsensorstate)
        || _last_tank_size != tank_size)
    {
        if ((material.empty() || bottlestate == 0) || (material == "unknown"))
        {
            _lbl_material_name->setText(lvTr("EMPTY_INFO"));

            _lbl_bottle_status->setText(lvTr("Status.Uninstalled"));
            _lbl_bottle_status->setTextClr(CLR_WARNING);
        } else
        {
            LOG_DEBUG() << "tank size: " << tank_size;
            _lbl_material_name->setText(material + " (" + std::to_string(tank_size / 1000) + "kg)");

            _lbl_bottle_status->setTextClr(CLR_ON_SURFACE);
            // _lbl_bottle_status->setText(std::to_string(remaining) + " g");
            _lbl_bottle_status->setText(true ? (std::to_string(remaining) + " g") : lvTrStr("EMPTY_INFO") + " g");
        }

        _last_material = material;

        if (bottlestate == 0) // bottlestate=0 时，材料名称不会及时刷新，下位机上报的问题， 这里兜底
        {
            _last_material = "unknown";
        }

        _last_remaining            = remaining;
        _last_bottlestate          = bottlestate;
        _last_printerthsensorstate = printerthsensorstate;
        _last_tank_size            = tank_size;
        _last_fill_equipped        = fill_equipped;

        // LOG_DEBUG() << "update bottle info: " << material << " " << tank_size << " " << remaining << " " << bottlestate << " " << printerthsensorstate;
        if (_resin_detail_dialog)
        {
            _resin_detail_dialog->UpdateBottleInfo(_last_material, _last_remaining, _last_bottlestate, _last_fill_equipped);
        }
    }

    // _slots_ProMachine = page::DataCenter::sigProMachine.connect([this](bool is_pro) { _is_pro = is_pro; });
}

PanelResinBottle::PanelResinBottle(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(331, 115);
    this->setOpacity(1);
    this->setBgColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER_HIGHEST));
    this->setRadius(8);

    constexpr short offset{40};
    constexpr short mod_item_edge_x{16};
    constexpr short mod_item_edge_row2_y{60};

    _icon_bottle = std::make_shared<widgets::Image>(":/image/36x36/mod_resin_bottle.png", this);
    _icon_bottle->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(18));

    _lbl_material_name = std::make_shared<ConsumeText>("", CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);

    if (sys::Application::getLanguageType() == sys::Application::LanguageType::English)
    {
        _lbl_material_name->setPos(lv_coord_t(mod_item_edge_x + offset), lv_coord_t(16));
    } else
    {
        _lbl_material_name->setPos(lv_coord_t(mod_item_edge_x + offset), lv_coord_t(18));
    }

    // lv_obj_set_width(_lbl_material_name->getLvglItem(), 270);
    // _lbl_material_name->setLongMode(1);
    constexpr int panel_text_max_length{270};
    _lbl_material_name->setWidth(panel_text_max_length);
    // _lbl_material_name->setPos(mod_item_edge_x + offset, 18);

    _lbl_bottle_status = std::make_shared<ConsumeText>("", CLR_ON_SURFACE, this, ConsumeText::FontSize22);
    _lbl_bottle_status->setAligment(LV_ALIGN_BOTTOM_LEFT, 16, -12);

    _touch_area = std::make_shared<widgets::MouseArea>(this);
    _touch_area->setSize(150, 92);
    _touch_area->setAligment(LV_ALIGN_RIGHT_MID, 0, 0);

    _touch_area->setOnClickedListener([this]() {
        LOG_DEBUG() << "  filling mod detail";

        if (!_resin_detail_dialog)
        {
            _resin_detail_dialog = std::make_shared<ModuleDetailResinDialog>("", _switch_enable);

            _resin_detail_dialog->setAutoDel(false);
        }

        _resin_detail_dialog->setSwitchStatus(_switch_enable, _fill_mod_equip);
        _resin_detail_dialog->UpdateBottleInfo(_last_material, _last_remaining, _last_bottlestate, _last_fill_equipped);

        _resin_detail_dialog->open();
    });

    _arrow_right = std::make_shared<widgets::Image>(":/image/32x32/arrow_right.png", this);
    _arrow_right->setSize(32, 32);
    _arrow_right->setAligment(LV_ALIGN_BOTTOM_RIGHT, -16, -15);

    static auto createProgressBar = [](BaseItem *parent) -> std::shared_ptr<wrapper::widgets::ProgressBar> {
        auto progress = std::make_shared<wrapper::widgets::ProgressBar>(parent, 120, 20, 0, 100, 0);
        progress->setBgRadius(4);
        progress->setIndicatorRadius(4);
        progress->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(mod_item_edge_row2_y + 10));

        progress->setMainColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER_LOWEST));
        progress->setIndicatorColor(design::ThemeColor(design::CLR_PROGRESS_NORMAL_FILL));

        return progress;
    };

    updateBottleInfo("", 0, 0, 0, 0, false);
}
} // namespace lvglpp::ui::page
