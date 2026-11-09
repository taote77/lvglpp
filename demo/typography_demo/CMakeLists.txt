project(typography_demo)

set(CMAKE_CXX_STANDARD 17)

# Generated role fonts, per-language tables and the role_fonts registry.
file(GLOB ROLE_FONT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n/fonts/*.c
)

add_executable(${PROJECT_NAME}
    main.cc
    TypographyDemoActivity.h
    TypographyDemoActivity.cc
    LvTranslator.h
    LvTranslator.cc
    i18n/translations/lv_i18n.c
    ${ROLE_FONT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n/fonts
    ${CMAKE_SOURCE_DIR}/framework/lvgl/
    ${CMAKE_SOURCE_DIR}/framework/lvgl/src/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    lvgl
)

# Manual validation target: parses the workbook's strings + fonts sheets and
# verifies the generated translations and role fonts are up to date.
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    add_custom_target(typography-check
        COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/framework/tools/i18n/gen_role_fonts.py
                --xlsx ${CMAKE_CURRENT_SOURCE_DIR}/i18n/strings.xlsx
                --out ${CMAKE_CURRENT_SOURCE_DIR}/i18n/fonts
                --prefix typography_demo --check
        COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/framework/tools/i18n/gen_lv_i18n.py
                --xlsx ${CMAKE_CURRENT_SOURCE_DIR}/i18n/strings.xlsx
                --out ${CMAKE_CURRENT_SOURCE_DIR}/i18n/translations/lv_i18n.c
                --check --strict
        COMMENT "Checking typography roles, translations and generated fonts"
        VERBATIM)
endif()

lvglpp_attach_assets(${PROJECT_NAME})
