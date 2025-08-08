
cmake_minimum_required(VERSION 3.31)

project(TodoPP)

set(TodoPP_VERSION_MAJOR 0)
set(TodoPP_VERSION_MINOR 1)
set(TodoPP_VERSION_PATCH 0)

set(TodoPP_SOURCES main.cpp todo.cpp task.cpp date.cpp)

add_executable(${PROJECT_NAME} ${TodoPP_SOURCES})

