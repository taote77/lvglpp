#ifndef LVGLPP_IMAGEBUTTON_H
#define LVGLPP_IMAGEBUTTON_H

#include "BaseItem.h"
#include <cstdint>
#include <string>

namespace lvglpp::widgets {

class ImageButton : public BaseItem
{
public:
    enum class State {
        Released        = 0,
        Pressed         = 1,
        Disabled        = 2,
        CheckedReleased = 3,
        CheckedPressed  = 4,
        CheckedDisabled = 5,
    };

    explicit ImageButton(BaseItem *parent = nullptr);

    /**
     * Set the three-part image sources for one state from application assets.
     * Empty strings pass nullptr. The decoded descriptors are retained as
     * object keepalives until the lv_obj is deleted.
     */
    ImageButton &setSource(State state, const std::string &left, const std::string &middle, const std::string &right);

    /// Same as setSource(), but the paths are real filesystem paths.
    ImageButton &setSourceFile(State state, const std::string &left, const std::string &middle, const std::string &right);

    ImageButton &setState(State state);

    State state() const
    {
        return state_;
    }

private:
    ImageButton &applySource(State state, const std::string &left, const std::string &middle, const std::string &right, bool fileSource);

    State state_ = State::Released;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_IMAGEBUTTON_H
