# CMakeLists.txt for all wrappers

if (ENABLE_LUA)
    # Add Lua wrapper
    add_subdirectory(lua)
else ()
    message(STATUS "Lua bindings disabled (enable with -DENABLE_LUA=ON)")
endif ()
