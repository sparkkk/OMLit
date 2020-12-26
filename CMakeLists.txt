cmake_minimum_required (VERSION 3.9)

project (OmLit)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

file(
    GLOB source_files
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/omlit/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/omlit/src/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/omlit/src/interface/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/omlit/src/lua/*.cpp"
)

include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/omlit/include"
)

add_compile_options(-std=c++17 -g)

add_link_options(-Wl,--export-all-symbols)

add_executable(TestOmLit ${source_files})

target_link_libraries(
    TestOmLit
    c++
    lua
    dl
)