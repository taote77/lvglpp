include_guard(GLOBAL)
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-std=c++17" HAS_CXX17)
check_cxx_compiler_flag("-std=c++14" HAS_CXX14)

if(HAS_CXX17)
    message(STATUS "using c++17")
    set(CMAKE_CXX_STANDARD 17)
elseif(HAS_CXX14)
    message(STATUS "using c++14")
    set(CMAKE_CXX_STANDARD 14)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/target/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/target/lib)
set(PLUGIN_OUT_PATH        ${PROJECT_BINARY_DIR}/target/plugin)
set(PROTOCOL_OUT_PATH      ${PROJECT_BINARY_DIR}/target/protocol)
set(SETUP_OUTPUT_PATH      ${PROJECT_BINARY_DIR}/setup)
set(SETUP_RESOURCE_PATH    ${PROJECT_SOURCE_DIR}/setup/resource)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/staticlib)

link_directories(${LIBRARY_OUTPUT_PATH})
link_directories(${PROTOCOL_OUT_PATH})
link_directories(${PROJECT_BINARY_DIR})
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

include_directories(${PROJECT_SOURCE_DIR}/src/lib/ucore/include)
include_directories(${PROJECT_SOURCE_DIR}/src/lib/libpx/include)
include_directories(${PROJECT_SOURCE_DIR}/src/lib/common/include)
include_directories(${PROJECT_SOURCE_DIR}/src/protocol)


#动态生成头文件包含目录
include_directories(${SETUP_OUTPUT_PATH})



add_compile_options(-Wno-psabi -g)
