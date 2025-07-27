project(smart-home-ui)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PRIVATE
    main.cc
    page/home/HomePage.h
    page/home/HomePage.cc
    page/home/Drawer.h
    page/home/Drawer.cc

)


add_definitions(-DUSED_SDL)

find_package(SDL2 REQUIRED SDL2)

target_include_directories(${PROJECT_NAME} PRIVATE
 ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    sqlite3 ${SDL2_LIBRARIES} ${Boost_LIBRARIES}
    # lvgl
)


target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)


SET(ASSET_SOURCE_DIR ${PROJECT_SOURCE_DIR}/asset)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SOURCE_DIR} ${EXECUTABLE_OUTPUT_PATH}/asset
)
