
## Solder Reflow Firmware
using ESP-IDF instead arduino based platform.


## Note 
Note Before First Build, remove **required** from lcdgfx `CMakeLists.txt`

```cmake

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS
                               "src"
                           REQUIRES
                               driver)
```

to 


```cmake

  idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS
                               "src")
```

## To Do Revision

- make all source for input 3.3v
- OLED_SDA and OLED_SCL is flipped
- remove series R21 and R20 from rotary encoder 

## Reference 

- [ESP8266 Toolchain Docs](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html#setup-toolchain)
- [Encoder.h](https://github.com/PaulStoffregen/Encoder/blob/c083e9cbd6400f7e72a794c0d371a00a09d2a25d/Encoder.h#L162)
