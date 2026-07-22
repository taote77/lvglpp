project(test)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_definitions(-DUSED_FRAMEBUFF)
else ()
    find_package(SDL2 REQUIRED SDL2)
    add_definitions(-DUSED_SDL)
endif ()

include(CheckCXXCompilerFlag)

# Visual test app (requires display)
add_executable(testd
    main.cc
    Home.cc Home.h
    BindDemo.h BindDemo.cc
    GameActivity.h GameActivity.cc
    GameStateDialog.h GameStateDialog.cc
)

target_link_libraries(testd PRIVATE
    lvglpp sqlite3 ${SDL2_LIBRARIES} ${Boost_LIBRARIES} lvgl
)

target_include_directories(testd PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/src/
    ${CMAKE_CURRENT_LIST_DIR}/../
    ${SDL2_INCLUDE_DIRS}
)

# Unit tests (logic only, no display needed)
add_executable(test_unit
    TestMain.cpp
    UnitTests.cpp
    WidgetTests.cpp
)

target_link_libraries(test_unit PRIVATE
    lvglpp lvgl ${SDL2_LIBRARIES}
)

target_include_directories(test_unit PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/src/
    ${CMAKE_CURRENT_LIST_DIR}/../
    ${SDL2_INCLUDE_DIRS}
)

# CTest integration
enable_testing()
add_test(NAME test_unit COMMAND test_unit)

