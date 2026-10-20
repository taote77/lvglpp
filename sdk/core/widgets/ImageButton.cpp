#include "ImageButton.h"
#include "core/base/LvObjFactory.h"
#include "core/tools/ImageManager.h"

#include <memory>
#include <utility>

namespace lvglpp::widgets {

ImageButton::ImageButton(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglImageButton, parent)
{
    setFocusable(true);
}

ImageButton &ImageButton::setSource(State state, const std::string &left, const std::string &middle, const std::string &right)
{
    return applySource(state, left, middle, right, false);
}

ImageButton &ImageButton::setSourceFile(State state, const std::string &left, const std::string &middle, const std::string &right)
{
    return applySource(state, left, middle, right, true);
}

ImageButton &ImageButton::applySource(State state, const std::string &left, const std::string &middle, const std::string &right, bool fileSource)
{
    if (getLvglItem() == nullptr)
    {
        return *this;
    }

    auto load = [fileSource](const std::string &path) -> tools::ImageHandle {
        if (path.empty())
        {
            return nullptr;
        }
        return fileSource ? tools::ImageManager::loadFile(path) : tools::ImageManager::load(path);
    };

    tools::ImageHandle leftHandle   = load(left);
    tools::ImageHandle middleHandle = load(middle);
    tools::ImageHandle rightHandle  = load(right);

    lv_imagebutton_set_src(getLvglItem(), static_cast<lv_imagebutton_state_t>(state), leftHandle ? leftHandle->descriptor() : nullptr,
                           middleHandle ? middleHandle->descriptor() : nullptr, rightHandle ? rightHandle->descriptor() : nullptr);

    const std::string prefix = "imagebutton.resource." + std::to_string(static_cast<int>(state)) + ".";
    setObjectKeepalive(prefix + "left", leftHandle);
    setObjectKeepalive(prefix + "middle", middleHandle);
    setObjectKeepalive(prefix + "right", rightHandle);
    return *this;
}

ImageButton &ImageButton::setState(State state)
{
    state_ = state;
    if (getLvglItem() != nullptr)
    {
        lv_imagebutton_set_state(getLvglItem(), static_cast<lv_imagebutton_state_t>(state));
    }
    return *this;
}

} // namespace lvglpp::widgets
