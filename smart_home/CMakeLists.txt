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
    theme/form_design.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
)

# Manual validation target: does not run as part of ALL (Python is optional).
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    add_custom_target(i18n-check
        COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/sdk/tools/i18n/gen_lv_i18n.py
                --xlsx ${CMAKE_CURRENT_SOURCE_DIR}/i18n/strings.xlsx
                --out ${CMAKE_CURRENT_SOURCE_DIR}/i18n/translations/lv_i18n.c
                --check --strict
        COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/sdk/tools/i18n/check_keys.py
                --xlsx ${CMAKE_CURRENT_SOURCE_DIR}/i18n/strings.xlsx
                --sources ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Checking smart_home i18n workbook, generated C and source keys"
        VERBATIM)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../sdk/lvgl/src/
    ${CMAKE_CURRENT_LIST_DIR}/../
)

lvglpp_attach_assets(${PROJECT_NAME})
