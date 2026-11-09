#ifndef SMART_HOME_SCENE_BUTTON_H
#define SMART_HOME_SCENE_BUTTON_H

#include "core/widgets/Widget.h"
#include "core/widgets/Label.h"
#include <functional>
#include <memory>
#include <string>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::widgets {

/// Large icon button for activating a scene
class SceneButton : public w::Widget
{
public:
    SceneButton(const std::string &icon, const std::string &name,
                const std::string &description, w::Widget *parent = nullptr);

    void setActive(bool active);
    bool isActive() const { return _active; }
    void setOnActivate(std::function<void()> cb) { _activateCb = std::move(cb); }

private:
    bool _active = false;
    std::function<void()> _activateCb;

    std::unique_ptr<w::Label> _iconLabel;
    std::unique_ptr<w::Label> _nameLabel;
    std::unique_ptr<w::Label> _descLabel;
};

} // namespace lvglpp::ui::widgets

#endif
