# Introduction 

SORE - Solder Reflow Firmware Version 2 

## How To build 

- make sure you install development environment as described in this [docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
- next add alias `alias get_idf='. $HOME/esp/esp-idf/export.sh'` to your shell profile (.zshrc or other) as described in this [docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
- next, you can start idf development by run `get_idf`
- to start project run `cmake -S . -B build` then build with `cmake --build build`
- next to upload with builded cmake project you can run `make -C build flash`
- to _serial monitor_ you can use `idf.py -p /dev/ttyUSB0 monitor`. note `/dev/ttyUSB0` is based on your system 
- to open menu config run `idf.py menuconfig`
- 🧴 **another option** to build project is use command `idf.py build`, this command will build project use ninja, also you can _flash_ with `idf.py -p PORT flash` PORT is usb name
