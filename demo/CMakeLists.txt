project(demo)




include(CheckCXXCompilerFlag)


set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PRIVATE
    main.cc
    BindDemo.h
    BindDemo.cc
    SnapShot.h
    SnapShot.cc
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    lvgl
)


include_directories(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)


add_subdirectory(i18n_demo)
add_subdirectory(qrcode)
add_subdirectory(remote_ctrl)
add_subdirectory(img_disp)
if (ENABLE_LUA)
    add_subdirectory(lua_ui)
endif ()
# add_subdirectory(clua)

lvglpp_attach_assets(${PROJECT_NAME})
