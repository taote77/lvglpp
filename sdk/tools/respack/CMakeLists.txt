add_executable(respack main.cpp)
target_compile_features(respack PRIVATE cxx_std_17)
target_include_directories(respack PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set_target_properties(respack PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
