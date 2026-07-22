project(smart-home-ui)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    main.cc

    # New smart home components
    components/DeviceCard.h
    components/DeviceCard.cc
    components/RoomCard.h
    components/RoomCard.cc
    components/ThermostatWidget.h
    components/ThermostatWidget.cc
    components/LightWidget.h
    components/LightWidget.cc
    components/SceneButton.h
    components/SceneButton.cc
    components/StatusBadge.h
    components/StatusBadge.cc
    components/EnergyChart.h
    components/EnergyChart.cc

    # Existing components
    components/screen_shoot.h
    components/screen_shoot.cc
    components/progress_bar.h
    components/progress_bar.cc
    components/LineEditor.h
    components/LineEditor.cc
    components/StringEditor.h
    components/StringEditor.cc
    components/ClickableCard.h
    components/ClickableCard.cc
    components/GroupCheckButton.h
    components/GroupCheckButton.cc
    components/ColLayout.h
    components/ColLayout.cc

    # Sensor pages (3D printer panel, preserved)
    page/sensor/SensorActivity.h
    page/sensor/SensorActivity.cc
    page/sensor/TempCurveDlg.h
    page/sensor/TempCurveDlg.cc

    # Drawer (preserved)
    page/drawer/TopDrawer.h
    page/drawer/TopDrawer.cc
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

    # i18n
    i18n/LvTranslator.h
    i18n/LvTranslator.cc
    i18n/translations/lv_i18n.h
    i18n/translations/lv_i18n.c

    # Theme
    theme/theme_keeper.h
    theme/theme_keeper.cc
    theme/form_design.h
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
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/src/
    ${CMAKE_CURRENT_LIST_DIR}/../
)

set(ASSET_SOURCE_DIR ${PROJECT_SOURCE_DIR}/asset)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SOURCE_DIR} ${EXECUTABLE_OUTPUT_PATH}/asset
)
