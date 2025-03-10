project(test )


include(CheckCXXCompilerFlag)

set(LVGLPP_TEST testd)

set(CMAKE_CXX_STANDARD 17)

add_executable(${LVGLPP_TEST})


file(GLOB_RECURSE GENERATED_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c *.cc *.cpp *.hpp *.h)
file(GLOB_RECURSE TRANS_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.c  *.cc *.cpp *.hpp *.h)


target_sources(${LVGLPP_TEST} PRIVATE
    main.cc
    lvglpp_application.h
    lvglpp_application.cc
    printer.h
    printer.cc
    home.cc
    home.h
    Home.cc
    Home.h
    private.h
    settings.cc
    settings.h
    stats.cc
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


# message("dd" ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/)


include_directories(${LVGLPP_TEST}
    ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)

