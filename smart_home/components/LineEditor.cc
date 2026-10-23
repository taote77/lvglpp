#include "LineEditor.h"
#include "core/sys/UiConfig.h"
#include "theme/form_design.h"
#include <algorithm>

namespace wrapper::widgets
{
    using namespace form;

    LineEditor::LineEditor(Widget *parent) : StringEditor(parent)
    {
        setSize(std::min(664, lvglpp::sys::UiConfig::instance().width() - 32), 60);
        setRadius(8);
        setBgColor(design::ThemeColor(design::CLR_ON_INFO));

        display_label_ = std::make_shared<Label>("", design::ThemeColor(design::CLR_ON_SURFACE), this, Label::FontSize28, Label::DMSans);
        display_label_->setAlignment(LV_ALIGN_CENTER, 0, 0);
        unit_label_ = std::make_shared<Label>("", design::ThemeColor(design::CLR_ON_SURFACE), this, Label::FontSize28, Label::DMSans);
        unit_label_->setAlignment(LV_ALIGN_RIGHT_MID, -20, 0);
    }

    void LineEditor::draw(const std::string &display)
    {
        display_label_->setText(display);
    }

    void LineEditor::setUnit(const std::string &str)
    {
        unit_str_ = str;
        unit_label_->setText(unit_str_);
    }

}// namespace wrapper::widgets
