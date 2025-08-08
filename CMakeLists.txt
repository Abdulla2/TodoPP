
cmake_minimum_required(VERSION 3.31)

project(TodoPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(TodoPP_VERSION_MAJOR 0)
set(TodoPP_VERSION_MINOR 1)
set(TodoPP_VERSION_PATCH 0)

set(TodoPP_SOURCES src/main.cpp src/todo.cpp src/task.cpp src/date.cpp)
set(TodoPP_INCLUDE_DIRS include)

configure_file(config.h.in config.h)

target_include_directories(${TodoPP_INCLUDE_DIRS} config.h)
add_executable(${PROJECT_NAME} ${TodoPP_SOURCES})

