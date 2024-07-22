# Introduction 

SORE - Solder Reflow Firmware Version 2 

## How To build 

- make sure you install development environment as described in this [docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
- next add alias `alias get_idf='. $HOME/esp/esp-idf/export.sh'` to your shell profile (.zshrc or other) as described in this [docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
- next, you can start idf development by run `get_idf`
- to start project run `cmake -S . -B build` then build with `cmake --build build`
