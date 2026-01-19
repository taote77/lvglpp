# CMakeLists.txt for all wrappers

# Add Lua wrapper
add_subdirectory(lua)

# Add Python wrapper
option(BUILD_PYTHON_WRAPPER "Build Python wrapper" ON)
if(BUILD_PYTHON_WRAPPER)
    add_subdirectory(python)
endif()

# Add common utilities
add_library(wrapper_common INTERFACE)

target_include_directories(wrapper_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)
