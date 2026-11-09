project(demo)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
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

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/framework/lvgl/
    ${CMAKE_SOURCE_DIR}/framework/lvgl/src/
)

lvglpp_attach_assets(${PROJECT_NAME})
