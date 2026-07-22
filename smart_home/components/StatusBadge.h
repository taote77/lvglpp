#ifndef SMART_HOME_STATUS_BADGE_H
#define SMART_HOME_STATUS_BADGE_H

#include "core/widgets/BaseItem.h"
#include <memory>
#include <string>

namespace lvglpp::ui::widgets {

/// Small colored dot + text indicating device status
class StatusBadge : public ::lvglpp::widgets::BaseItem
{
public:
    enum State { Online, Offline, Warning, Error };

    explicit StatusBadge(State state = Online, ::lvglpp::widgets::BaseItem *parent = nullptr);

    void setState(State state);

    State state() const { return _state; }

    void setLabel(const std::string &text);

private:
    void _updateAppearance();

    State _state = Online;
    std::unique_ptr<::lvglpp::widgets::BaseItem> _dot;
    std::unique_ptr<::lvglpp::widgets::BaseItem> _label;
};

} // namespace lvglpp::ui::widgets

#endif
