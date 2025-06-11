project(clua)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    main.cc
)


target_include_directories(${PROJECT_NAME} PRIVATE
    ./    
    include/
)
