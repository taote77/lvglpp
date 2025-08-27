#include "DetailResinDialog.h"
#include "core/log/log.h"
#include "core/widgets/Toast.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <cstdint>

namespace lvglpp::ui::page {
using namespace form::design;
using namespace lvglpp;

constexpr uint32_t   DIALOG_CONTENT_WIDTH{560};
constexpr uint32_t   DIALOG_CONTENT_HEIGHT{300};
constexpr lv_coord_t DIALOG_CONTENT_RADIUS{20};

ModuleDetailResinDialog::ModuleDetailResinDialog(const std::string &resin_name, bool mod_switch) : BaseDialog(true), _change_dir_cb(nullptr), _cancel_cb(nullptr)
{
    _dialog_bg_item = std::make_shared<BaseItem>(this->getRootItem());
    _dialog_bg_item->setSize(DIALOG_CONTENT_WIDTH, DIALOG_CONTENT_HEIGHT);
    _dialog_bg_item->setRadius(DIALOG_CONTENT_RADIUS);
    _dialog_bg_item->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));
    _dialog_bg_item->setAligment(LV_ALIGN_TOP_MID, 0, 72);

    std::string material_str = resin_name.empty() ? lvTr("EMPTY_INFO") : resin_name;
    _material_name_label     = std::make_shared<ConsumeText>(material_str, ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24Bold);
    _material_name_label->setAligment(LV_ALIGN_TOP_LEFT, 24, 20);

    _btn_label = std::make_shared<ConsumeText>(lvTr("Panel.Autofill.Label"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _btn_label->setPos(24, 82);

    _auto_fill_switch = std::make_shared<widgets::SwitchButton>(72, 36, _dialog_bg_item.get());

    _auto_fill_switch->setVisible(false);

    _auto_fill_switch->setPos(448, 82);
    _auto_fill_switch->setCheck(mod_switch);
    _auto_fill_switch->setOnClickedListener([this]() -> void {
        LOG_INFO() << "open filling mod";

        if (!_fill_mod_equipped && !_auto_fill_switch->isCheck())
        {
            widgets::Toast::success(lvTr("CheckFillModTips"));
            return;
        }

        // comm::OtherComm::SetAutomaticFillingEnable(!_auto_fill_switch->isCheck());
        _auto_fill_switch->setCheck(!_auto_fill_switch->isCheck());

        std::string msg = _auto_fill_switch->isCheck() ? "AutoFillOpenTips" : "AutoFillCloseTips";

        widgets::Toast::warn(lvTr(msg.c_str()));
    });

    _status_label = std::make_shared<ConsumeText>(lvTr("Panel.Resin.Remaining"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _status_label->setPos(24, 82 + 32 + 15);

    _remaing_info = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _remaing_info->setAligmentTo(*_status_label, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

    _close_button = std::make_shared<RoundedButton>(48, 48, RoundedButton::ColorStyle::Gray, "", ":/image/32x32/x_close.png", _dialog_bg_item.get());
    _close_button->setRadius(24);
    _close_button->setAligment(LV_ALIGN_BOTTOM_MID, 0, -20);
    _close_button->setOnClickedListener([this]() {
        LOG_DEBUG() << "close mod detail dialog";
        this->close();
    });

    // _slots_fill_heat_status = page::DataCenter::sigModSwitchStatus.connect([this](int mod_type, bool enable, bool equipped) {
    //     LOG_DEBUG() << "sdsdds" << mod_type << enable << equipped;
    //     if (mod_type == 0)// filling
    //     {
    //         _fill_mod_equipped = equipped;
    //     }
    //     else if (mod_type == 1)// heating
    //     {
    //         // fallthrough
    //     }
    // });
}

void ModuleDetailResinDialog::setSwitchStatus(bool mod_switch, bool equipped)
{
    if (_auto_fill_switch)
    {
        _auto_fill_switch->setCheck(mod_switch);

        _auto_fill_switch->setEnable(equipped);
    }
}

void ModuleDetailResinDialog::UpdateBottleInfo(const std::string &resin_name, const int32_t remaining, const int32_t bottlestate, bool fillmod_equipped)
{
    _material_name_label->setText((resin_name.empty() || resin_name == "unknown") ? lvTr("EMPTY_INFO") : resin_name);

    _fill_mod_equipped = fillmod_equipped;

    if (bottlestate == 1)
    {
        try
        {
            _remaing_info->setText(" " + std::to_string(remaining) + " g");
        }
        catch (const std::exception &e)
        {
            LOG_ERROR() << "UpdateBottleInfo error: " << e.what();
        }

        // _rounded_button->setEnable(true);
    } else
    {
        _remaing_info->setText(lvTr("EMPTY_INFO"));
        // _rounded_button->setEnable(false);
    }

    if (_btn_label)
    {
        _btn_label->setVisible(_fill_mod_equipped);
    }

    if (_status_label)
    {
        _status_label->setVisible(_fill_mod_equipped);
    }

    if (_remaing_info)
    {
        _remaing_info->setVisible(_fill_mod_equipped);
    }
}

ModuleDetailResinDialog::~ModuleDetailResinDialog()
{
    // _slots_fill_heat_status.disconnect();
}

void ModuleDetailResinDialog::open()
{
    _status_label->setText(lvTr("Panel.Resin.Remaining"));
    _btn_label->setText(lvTr("Panel.Autofill.Label"));
    // _remaing_info
    BaseDialog::open();
}

void ModuleDetailResinDialog::close()
{
    BaseDialog::close();
}

} // namespace lvglpp::ui::page
