#ifndef LINE_EDITOR_H
#define LINE_EDITOR_H

#include "StringEditor.h"
#include "core/widgets/Label.h"
#include <memory>

namespace wrapper::widgets {
class LineEditor : public StringEditor
{
public:
    explicit LineEditor(Widget *parent = nullptr);

    void setUnit(const std::string &str);

protected:
    void draw(const std::string &display) override;

private:
    std::shared_ptr<Label> display_label_;
    std::shared_ptr<Label> unit_label_;
    std::string                  unit_str_;
};

} // namespace wrapper::widgets

#endif // LINE_EDITOR_H
