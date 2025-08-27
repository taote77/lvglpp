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
    page/sensor/SensorActivity.h
    page/sensor/SensorActivity.cc
    page/sensor/TempCurveDlg.h
    page/sensor/TempCurveDlg.cc
    page/drawer/TopDrawer.h
    page/drawer/TopDrawer.cc
    page/drawer/SwipeView.cc
    page/drawer/SwipeView.cc
    page/drawer/MessageCenterControl.cc
    page/drawer/MessageCenterControl.h
    page/drawer/ModuleCollector.cc
    page/drawer/ModuleCollector.h
    page/drawer/module_panel/DetailPulseDialog.cc
    page/drawer/module_panel/DetailPulseDialog.h
    page/drawer/module_panel/DetailResinDialog.cc
    page/drawer/module_panel/DetailResinDialog.h
    page/drawer/module_panel/PanelHeater.cc
    page/drawer/module_panel/PanelHeater.h
    page/drawer/module_panel/PanelLCD.cc
    page/drawer/module_panel/PanelLCD.h
    page/drawer/module_panel/PanelPulseMod.cc
    page/drawer/module_panel/PanelPulseMod.h
    page/drawer/module_panel/PanelPulseTray.cc
    page/drawer/module_panel/PanelPulseTray.h
    page/drawer/module_panel/PanelResinBottle.cc
    page/drawer/module_panel/PanelResinBottle.h
    page/drawer/module_panel/PanelTempHumi.cc
    page/drawer/module_panel/PanelTempHumi.h
    page/drawer/NoticeDelegate.cc
    page/drawer/NoticeDelegate.h
    page/drawer/RebootConfirmDialog.cpp
    page/drawer/RebootConfirmDialog.h
    page/drawer/SwipeView.cc
    page/drawer/SwipeView.h
    i18n/LvTranslator.h
    i18n/LvTranslator.cc
    i18n/translations/lv_i18n.h
    i18n/translations/lv_i18n.c
    theme/theme_keeper.h
    theme/theme_keeper.cc
    theme/form_design.h
    boost/screen_shoot.h
    boost/screen_shoot.cc
    boost/progress_bar.h
    boost/progress_bar.cc
    boost/LineEditor.h
    boost/LineEditor.cc
    boost/StringEditor.h
    boost/StringEditor.cc
    boost/ClickableCard.h
    boost/ClickableCard.cc
    boost/GroupCheckButton.h
    boost/GroupCheckButton.cc
    boost/ColLayout.h
    boost/ColLayout.cc
)


add_definitions(-DUSED_SDL)

find_package(SDL2 REQUIRED SDL2)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}
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
