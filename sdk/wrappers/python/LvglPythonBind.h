#ifndef LVGLPP_LVGL_PYTHON_BIND_H
#define LVGLPP_LVGL_PYTHON_BIND_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include "core/widgets/BaseItem.h"

namespace py = pybind11;
namespace lvglpp::python {

class LvglPythonBind {
public:
    LvglPythonBind();
    ~LvglPythonBind();
    
    void initialize();
    void shutdown();
    
    py::module& getPythonModule();
    
    bool executeScript(const std::string& script);
    bool executeFile(const std::string& filename);
    
    void setRootItem(std::shared_ptr<widgets::BaseItem> root);
    std::shared_ptr<widgets::BaseItem> getRootItem() const;
    
private:
    void registerPythonAPI();
    
    std::unique_ptr<py::module> python_module_;
    std::shared_ptr<widgets::BaseItem> root_item_;
    bool initialized_;
};

} // namespace lvglpp::python

#endif // LVGLPP_LVGL_PYTHON_BIND_H
