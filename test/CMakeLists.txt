project(test)

set(CMAKE_CXX_STANDARD 17)


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
    lvglpp lvgl
)

target_include_directories(testd PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/src/
    ${CMAKE_CURRENT_LIST_DIR}/../
)

# Unit tests (logic only, no display needed)
add_executable(test_unit
    TestMain.cpp
    UnitTests.cpp
    WidgetTests.cpp
    LifecycleTests.cpp
    StyleLayoutTests.cpp
    EventInputTests.cpp
    TimerAnimationTests.cpp
    NewWidgetTests.cpp
    ResourceTests.cpp
    PageTests.cpp
    AssetPackTests.cpp
    AssetManagerTests.cpp
    ApplicationTests.cpp
    FontCoverageTests.cpp
    UiConfigTests.cpp
    RtlLayoutTests.cpp
)

target_link_libraries(test_unit PRIVATE
    lvglpp lvgl
)

target_include_directories(test_unit PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/src/
    ${CMAKE_CURRENT_LIST_DIR}/../
)

# Sanitizer flags are set in sdk/ scope, which does not propagate to this
# directory. Mirror them here so test_unit is instrumented too.
if (ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(test_unit PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(test_unit PRIVATE -fsanitize=address)
    target_compile_options(testd PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(testd PRIVATE -fsanitize=address)
endif ()

# CTest integration
enable_testing()
add_test(NAME test_unit COMMAND test_unit)
set_tests_properties(test_unit PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/target/bin")

