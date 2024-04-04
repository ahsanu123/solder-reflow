set(component_srcs "ssd1306.c" )

# get IDF version for comparison
set(idf_version "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")

if(idf_version VERSION_GREATER_EQUAL "5.2")
  message("-- using new i2c driver")
  list(APPEND component_srcs "ssd1306_i2c_new.c")
else()
  message("-- using LEGACY i2c driver")
  list(APPEND component_srcs "ssd1306_i2c_legacy.c")
endif()

idf_component_register(SRCS "${component_srcs}" INCLUDE_DIRS ".")
