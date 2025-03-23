project(test)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_definitions(-DUSED_FRAMEBUFF)
    #add_definitions(-DUSED_WAYLAND)
else ()
    find_package(SDL2 REQUIRED SDL2)
    add_definitions(-DUSED_SDL)
    include_directories(${SDL2_INCLUDE_DIRS})
endif ()


include(CheckCXXCompilerFlag)

set(LVGLPP_TEST testd)

set(CMAKE_CXX_STANDARD 17)

add_executable(${LVGLPP_TEST})


target_sources(${LVGLPP_TEST} PRIVATE
    main.cc
    Home.cc
    Home.h
)


# file(GLOB_RECURSE GENERATED_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c *.cc *.cpp *.hpp *.h)
# file(GLOB_RECURSE TRANS_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.c  *.cc *.cpp *.hpp *.h)


target_link_libraries(${LVGLPP_TEST} PRIVATE
    lvglpp
    sqlite3 ${SDL2_LIBRARIES} ${Boost_LIBRARIES}
    # lv_i18n
    lvgl
    # lvgl::examples
    # lvgl::demos
)


include_directories(${LVGLPP_TEST}
    # ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)

