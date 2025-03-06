project(test)

set(LVGLPP_TEST testd)

add_executable(${LVGLPP_TEST})


file(GLOB_RECURSE GENERATED_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c *.cpp *.hpp *.h)
file(GLOB_RECURSE TRANS_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.c *.cpp *.hpp *.h)


target_sources(${LVGLPP_TEST} PRIVATE
    main.cc
    printer.c
    printer.h
    home.cc
    home.h
    private.h
    settings.c
    settings.h
    stats.c
    stats.h
    ${GENERATED_SRC}
    ${TRANS_SRC}
)

target_link_libraries(${LVGLPP_TEST} PRIVATE
    lvglpp
    # lv_i18n
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
