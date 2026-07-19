#include "DetailPulseDialog.h"
#include "components/progress_bar.h"
#include "core/log/log.h"
#include "core/widgets/PushButton.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <cstdint>

namespace lvglpp::ui::page {
using namespace form::design;
using namespace lvglpp;

constexpr uint32_t   DIALOG_CONTENT_WIDTH{560};
constexpr uint32_t   DIALOG_CONTENT_HEIGHT{300};
constexpr lv_coord_t DIALOG_CONTENT_RADIUS{20};

ModuleDetailPulseDialog::ModuleDetailPulseDialog(const int progress, const int status) : BaseDialog(true), _change_dir_cb(nullptr), _cancel_cb(nullptr)
{
    _dialog_bg_item = std::make_shared<BaseItem>(this->getRoot());
    _dialog_bg_item->setSize(DIALOG_CONTENT_WIDTH, DIALOG_CONTENT_HEIGHT);
    _dialog_bg_item->setRadius(DIALOG_CONTENT_RADIUS);
    _dialog_bg_item->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));

    _dialog_bg_item->setAligment(LV_ALIGN_TOP_MID, 0, 72);

    _title_label = std::make_shared<ConsumeText>(lvTr("ElectronStack"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24Bold);
    _title_label->setAligment(LV_ALIGN_TOP_LEFT, 24, 20);

    _title_status_label = std::make_shared<ConsumeText>(lvTr("Status.Abnormal"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24Bold);
    _title_status_label->setAligmentTo(*_dialog_bg_item.get(), LV_ALIGN_TOP_RIGHT, -24, 20);

    _progress_label = std::make_shared<ConsumeText>(lvTr("ModUsed"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _progress_label->setAligmentTo(*_title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 24);

    _progress_bar = std::make_shared<ProgressBar>(_dialog_bg_item.get(), 120, 20);
    _progress_bar->setAligmentTo(*_progress_label, LV_ALIGN_OUT_RIGHT_MID, 12, 0);
    _progress_bar->updateValue(21);
    _progress_bar->setBgRadius(4);
    _progress_bar->setIndicatorRadius(4);

    _progress_bar->setMainColor(ThemeColor(CLR_SURFACE_CONTAINER_LOWEST));
    _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));

    _status_label = std::make_shared<ConsumeText>(lvTr("Panel.Resin.Remaining"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _status_label->setPos(24, 82 + 32 + 15);

    _status_value_label = std::make_shared<ConsumeText>(lvTr(""), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    // _status_value_label = std::make_shared<ConsumeText>(lvTr("EMPTY_INFO"), ThemeColor(CLR_ON_SURFACE), _dialog_bg_item.get(), ConsumeText::FontSize24);
    _status_value_label->setAligmentTo(*_status_label, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

    _close_button = std::make_shared<RoundedButton>(48, 48, RoundedButton::ColorStyle::Gray, "", ":/image/32x32/x_close.png", _dialog_bg_item.get());
    _close_button->setRadius(24);
    _close_button->setAligment(LV_ALIGN_BOTTOM_MID, 0, -20);
    _close_button->setOnClickedListener([this]() {
        LOG_DEBUG() << "close mod detail dialog";
        this->close();
    });
}

void ModuleDetailPulseDialog::updatePulseFilmInfo(double usedtime, int32_t tank_type, int32_t airmod_state)
{
    if (tank_type == 1)
    {
        _title_label->setText(lvTr("ElectronStack"));
    } else
    {
        _title_label->setText(lvTr("EMPTY_INFO"));
    }

    std::string status_str;

    if (airmod_state == 1)
    {
        status_str = "Status.Normal";
        _title_status_label->setTextClr(CLR_ON_INFO);

        _progress_bar->setVisible(true);

        if (usedtime < 2500)
        {
            _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_NORMAL_FILL));
        } else if (usedtime > 3000)
        {
            _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_WARN_FILL));
        } else
        {
            _progress_bar->setIndicatorColor(ThemeColor(CLR_PROGRESS_ERROR_FILL));
        }

        double percent = usedtime / 3000.0;
        _progress_bar->updateValue(percent);

    } else
    {
        status_str = "Status.Abnormal";
        _title_status_label->setTextClr(CLR_WARNING);

        _progress_bar->setVisible(false);
    }

    _title_status_label->setText(lvTr(status_str.c_str()));
}

ModuleDetailPulseDialog::~ModuleDetailPulseDialog()
{}

void ModuleDetailPulseDialog::open()
{
    BaseDialog::open();
}

void ModuleDetailPulseDialog::close()
{
    BaseDialog::close();
}

} // namespace lvglpp::ui::page
