# CMakeLists.txt for Python wrapper

project(lvglpp_python)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pybind11
find_package(pybind11 REQUIRED)

# Create Python module with a unique name to avoid conflict
pybind11_add_module(lvglpy SHARED)

target_sources(lvglpy PRIVATE
    LvglPythonBind.cc
    WidgetPythonBind.cc
    PythonUtils.cc
)

target_include_directories(lvglpy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sdk/core
    ${CMAKE_SOURCE_DIR}/sdk/lvgl
)

target_link_libraries(lvglpy PRIVATE
    lvglpp
    lvgl
)

# Set output directory for Python module
set_target_properties(lvglpy PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python
)
