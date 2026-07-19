#include "core/base/LvObjFactory.h"
#include "LvChart.h"

namespace lvglpp::widgets {

LvChart::LvChart(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglChart, parent)
{}

} // namespace lvglpp::widgets