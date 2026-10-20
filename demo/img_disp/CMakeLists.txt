project(img_disp)




include(CheckCXXCompilerFlag)


set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PRIVATE
    main.cc
    Widget.h
    Widget.cc
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    lvgl
)


include_directories(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)

lvglpp_attach_assets(${PROJECT_NAME})
