# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_definitions(-DESP_LVGL_PORT_BUTTON_COMPONENT )
add_compile_definitions(ESP_LVGL_PORT_BUTTON_COMPONENT  ON)

project(SORE_V2)
