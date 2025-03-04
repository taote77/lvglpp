project(test)

set(LVGLPP_TEST testd)

add_executable(${LVGLPP_TEST})


target_sources(${LVGLPP_TEST} PRIVATE
    main.cc
)

target_link_libraries(${LVGLPP_TEST} PRIVATE
    lvglpp
    # lvgl
    # lvgl::examples
    # lvgl::demos
)


message("dd" ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/)


include_directories(${LVGLPP_TEST}
    ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)
