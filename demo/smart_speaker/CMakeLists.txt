project(smart_speaker)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    main.cc
    SmartSpeakerActivity.h
    SmartSpeakerActivity.cc
    FaceTypes.h
    FacePage.h
    FacePage.cc
    CandlePage.h
    CandlePage.cc
    FacePanel.h
    FacePanel.cc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    lvgl
)

include_directories(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/../../framework/lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../../framework/lvgl/src/
)
