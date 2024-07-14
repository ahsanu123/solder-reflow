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

add_compile_definitions(
  -D__XTENSA__
  -DCONFIG_ESP8266_I2C_AVAILABLE
  -DCONFIG_ESP8266_I2C_ENABLE

  # -DCONFIG_INTERFACE=I2C_INTERFACE
  # -DCONFIG_PANEL=SSD1306_128x32
  # -DCONFIG_OFFSETX=0
  # -DCONFIG_FLIP=false
  # -DCONFIG_SDA_GPIO=GPIO_NUM_4
  # -DCONFIG_SCL_GPIO=GPIO_NUM_5
  # -DCONFIG_RESET_GPIO=
  # -DCONFIG_MOSI_GPIO=
  # -DCONFIG_SCLK_GPIO=
  # -DCONFIG_CS_GPIO=
  # -DCONFIG_DC_GPIO=
  )
message("-- IDF Version: ${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")

project(solder_reflow VERSION 0.1)

