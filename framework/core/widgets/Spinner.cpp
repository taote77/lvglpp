#include "Spinner.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Spinner::Spinner(Widget *parent) : Widget(tools::LvObjFactory::createLvglSpinner, parent)
{}

Spinner &Spinner::setAnimParams(uint32_t durationMs, uint32_t arcAngle)
{
    if (lvObj() != nullptr)
    {
        lv_spinner_set_anim_params(lvObj(), durationMs, arcAngle);
    }
    return *this;
}

} // namespace lvglpp::widgets
