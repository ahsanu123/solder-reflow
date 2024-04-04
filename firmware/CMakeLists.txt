# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# adding new library like ssd1306 seem not work cause
# esp8266 still in migrate to new style esp-idf build system

# set(CMAKE_EXPORT_COMPILE_COMMANDS true)
# set(ENV{ESPPORT} /dev/ttyUSB0)
# set(IDF_TARGET "esp8266")
#
# include(FetchContent)
# FetchContent_Declare(
#   ssd1306
#   GIT_REPOSITORY https://github.com/nopnop2002/esp-idf-ssd1306.git
#   )
# message("-- Downloading ssd1306")
# FetchContent_MakeAvailable(ssd1306)
#
# set(EXTRA_COMPONENT_DIRS "${ssd1306_SOURCE_DIR}/components/ssd1306")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(solder_reflow)

