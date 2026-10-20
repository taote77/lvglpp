#include "Spinner.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Spinner::Spinner(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglSpinner, parent)
{}

Spinner &Spinner::setAnimParams(uint32_t durationMs, uint32_t arcAngle)
{
    if (getLvglItem() != nullptr)
    {
        lv_spinner_set_anim_params(getLvglItem(), durationMs, arcAngle);
    }
    return *this;
}

} // namespace lvglpp::widgets
