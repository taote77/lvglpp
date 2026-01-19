#ifndef LVGLPP_WIDGET_PYTHON_BIND_H
#define LVGLPP_WIDGET_PYTHON_BIND_H

#include <pybind11/pybind11.h>
#include <memory>
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/Image.h"
#include "core/widgets/PushButton.h"

namespace py = pybind11;
namespace lvglpp::python {

class WidgetPythonBind {
public:
    static void bindBaseItem(py::module& m);
    static void bindLvText(py::module& m);
    static void bindImage(py::module& m);
    static void bindPushButton(py::module& m);
    
    static void bindAll(py::module& m);
    
private:
    template<typename T>
    static void bindCommonMethods(py::class_<T>& cls);
};

} // namespace lvglpp::python

#endif // LVGLPP_WIDGET_PYTHON_BIND_H
