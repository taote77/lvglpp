# Demo applications. Each target lives in its own subdirectory with its own
# CMakeLists.txt; this file only aggregates them.

add_subdirectory(showcase)
add_subdirectory(i18n_demo)
add_subdirectory(qrcode)
add_subdirectory(remote_ctrl)
add_subdirectory(img_disp)
add_subdirectory(smart_speaker)
add_subdirectory(smart_home)
add_subdirectory(typography_demo)
if (ENABLE_LUA)
    add_subdirectory(lua_ui)
endif ()
# add_subdirectory(clua)
