project(i18n_demo)




include(CheckCXXCompilerFlag)


set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})

set(TR_SRC
    translations/lv_i18n.h
    translations/lv_i18n.c
)

target_sources(${PROJECT_NAME} PRIVATE
    main.cc
    LangChange.h
    LangChange.cc
    LvTranslator.h
    LvTranslator.cc
    ${TR_SRC}
    I18nWidgets/InText.h
    I18nWidgets/InText.cc
    session.h
    session.cc
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    lvgl
)


include_directories(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ./    
)

lvglpp_attach_assets(${PROJECT_NAME})
