cmake_minimum_required(VERSION 3.15)

set(NAME obj)
project(${NAME})

find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
endif()

add_executable(${NAME}
    src/main.cpp
    src/common.hpp)

target_link_libraries(${NAME} glfw)
target_link_libraries(${NAME} glad::glad)