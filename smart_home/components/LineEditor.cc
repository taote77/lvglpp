#include "LineEditor.h"
#include "theme/form_design.h"

namespace wrapper::widgets
{
    using namespace form;

    LineEditor::LineEditor(BaseItem *parent) : StringEditor(parent)
    {
        setSize(664, 60);
        setRadius(8);
        setBgColor(design::ThemeColor(design::CLR_ON_INFO));

        display_label_ = std::make_shared<ConsumeText>("", design::ThemeColor(design::CLR_ON_SURFACE), this, ConsumeText::FontSize28, ConsumeText::DMSans);
        display_label_->setAligment(LV_ALIGN_CENTER, 0, 0);
        unit_label_ = std::make_shared<ConsumeText>("", design::ThemeColor(design::CLR_ON_SURFACE), this, ConsumeText::FontSize28, ConsumeText::DMSans);
        unit_label_->setAligment(LV_ALIGN_RIGHT_MID, -20, 0);
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
