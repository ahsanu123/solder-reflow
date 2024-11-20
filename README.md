
 <h1 align="center">🌕 SORE</h1>
<p align="center">The DIY Solder Reflow 
<p align="center">
  <a href="https://github.com/ahsanu123/solder-reflow">
   <img src="./docs/SoreBannerV2.png" alt="SoreIcon">
  </a>
</p>

<p align="center">
  <a href="">
    <img alt="npm version" src="https://badgen.net/github/commits/ahsanu123/solder-reflow/">
  </a>
  <a href="">
    <img alt="npm" src="https://badgen.net/github/contributors/ahsanu123/solder-reflow">
  </a>
  <a href="">
    <img alt="npm" src="https://badgen.net/github/branches/ahsanu123/solder-reflow">
  </a>
  <a href="https://github.com/ahsanu123/solder-reflow/blob/main/LICENSE">
    <img alt="licence" src="https://badgen.net/github/license/ahsanu123/solder-reflow">
  </a>
</p>

## ✏️ SORE - Solder Reflow

🌕 SORE - Solder Reflow,  DIY hot plate made from several reference across internet, 
use PT100 for temperature sensor controlled with ESP32-WROOM-32D, with 1.47 IPS LCD, 
programmed in ESP IDF Framework with LVGL for display. 

> Designed With [KICAD](https://www.kicad.org)

<p align="center" style="background-color: white;">
<img alt="schematic" src="./hardware/board/docs/solder-reflow.svg">
</p>

## 🥈 Version 2.0 - Main Info

<p align="center" style="background-color: white;">
<img alt="schematic" src="./docs/solder-reflow-pcb-v2.png">
</p>

> - 🥑 Built In 5V and 3.3v Voltage Source
> - 0️⃣ Built in SSR Zero Crossing
> - ⛓️ Built In USB Serial Interface
> - 🎣 Use TL074 For PT100 Signal Conditioning
> - 🌮 5 Programmable User Input
> - 🍒 Use ESP32-Wroom
> - 🥓 10 Pin JTAG Connector
> - 🧢 0.91' OLED or 1.3' TFT

## 🖍️ Plot of Sampled Data

<details>
 <summary>
  Plot Collection
 </summary>
 
- multiple sampling at 2Mhz (except first one)
signal was moving averaged filter, first graph was actually noisy, and rest was actually not really noisy, but after filtered low noise signal look bumped
![image](./docs/multiple_plot_with_diference_pwm_duty_cycle.png)

- Transfer Function Estimation
$`y(t) = 350(1-e^{(t-d)/tau})`$ in temperature degre (celcius), d = 17.5 second
![image](./docs/transfer_function_estimation.png)

- first time sampled diagram [30 oktober 2024]
![image](https://github.com/user-attachments/assets/2c9f545f-8a4b-424a-9164-6b96f2deb1fe)

</details>

## 🐞 Debugging 
there is many way to debug ESP32 use JTAG, refer to [documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/jtag-debugging/index.html). 
at this time i only got JLink V8(2014),

🧰 Requirements 
> xtensa-esp32-elf toolchain  
> openocd-esp32 (fork of official openocd)  
> JLink  

🩹 Step To Debug 
1. run `get_idf` to start esp idf environtment (if openocd cant connect to esp32, try to serial monitor first before connecting with openocd, `idf.py -p /dev/ttyUSB0 monitor`) 
2. connect your board with jlink like this [ref](https://gojimmypi.github.io/ESP32-JTAG-GDB-Debugging/) (make sure your JLink and esp32 power up
```txt
TDI -> GPIO12
TCK -> GPIO13
TMS -> GPIO14
TDO -> GPIO15
TRST -> EN / RST (Reset)
GND -> GND
```
2. start openocd-esp32 (use sudo if needed) `sudo openocd -f interface/jlink.cfg -c "adapter speed 500" -f target/esp32.cfg`
3. next start your esp32 gdb  `xtensa-esp32-elf-gdb path/to/your/name.elf`
4. inside gdb cli [see this why connecting twice](https://esp32.com/viewtopic.php?t=9719#p40510)
```shell
target remote localhost:3333 
target remote localhost:3333 // dont know esp32 connected after try to connecting twice (doesn't need do twice if already connected 
```

5. **anyway** esp-idf (idf.py) got built in to start [gdbgui](gdbgui.com), just run `idf.py gdbgui`, idf will open `localhost:5000` (success with installed gdbgui version 0.15.2.0)

```shell
$ idf.py gdbgui
Executing action: gdbgui
Running:  ['gdbgui', '-g', 'xtensa-esp32-elf-gdb -x=/home/ahsanu/project/2024/hardware/solder-reflow/firmware/v2/build/gdbinit/gdbinit']
gdbgui started as a background task 363884
Executing action: post_debug
```
 
6. Enjoy debugging 🐞 😙
---

🗒️ **svd explorer**  
you can use xmlstartlet `xml` to see what inside svd, ex(this command will list all property inside svd):  
```shell
xml el /path/to/your/name.svd 
```
_svd file is xml file_

🍾 **example gdb remote connection success**
```shell
(gdb) target remote localhost:3333
Remote debugging using localhost:3333
0x400845e6 in esp_cpu_wait_for_intr () at /home/ahsanu/esp/esp-idf/components/esp_hw_support/cpu.c:145 145}
```
🍾 **example openocd connection success**
```shell
$ sudo openocd -f interface/jlink.cfg -c "adapter speed 500" -f target/esp32.cfg
Open On-Chip Debugger v0.12.0-esp32-20240318 (2024-03-18-18:25)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
adapter speed: 500 kHz
Info : auto-selecting first available session transport "jtag". To override use 'transport select <transport>'.
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : J-Link ARM V8 compiled Nov 28 2014 13:44:46
Info : Hardware version: 8.00
Info : VTarget = 3.371 V
Info : clock speed 500 kHz
Info : JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : JTAG tap: esp32.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : [esp32.cpu0] Examination succeed
Info : [esp32.cpu1] Examination succeed
Info : starting gdb server for esp32.cpu0 on 3333
Info : Listening on port 3333 for gdb connections
```

## 🌱 V2 Project Progress

<details>
 <summary>
  Show Picture 
 </summary>
 <p align="center">
  <a href="https://github.com/ahsanu123/solder-reflow">
   <img src="./docs/Sudi-pic1.jpg" alt="sore picture">
  </a>
</p>
</details>


- PCB Design Finished 10 june 2024 20:51
- Order _BOM_ and PCB
- PCB and BOM Arrive 12 july 2024 about 16:00
- Start to soldering 13 july 2024 about 06:00 (**weekend**)
- make adc class to simplify adc initialization 23 july 2024 at 21:21
- Check Out GPIO (Worked!!😃) 26 july 2024 at 22:24
- Try to use JLink for jtag debugging, but fail 😞 , 27 july 2024 at 17:30
- Try again to use JLink for jtag debugging, Worked!! 😃 , 28 july 2024 at 11:36
- Add `Button` class to add Input with callback, 29 Agustus 2024 at 19:42
- ⚠️ turn out, TL074 (not TL074xH) need minimum 10v to operate and not single supply quad opamp 😞, fortunately there is LM324 (same package and single supply support), currently still in shipping, need to wait if it really work or not, 01 september 2024 at 18:05
- ✔️ ok LM324 worked, currently still investigate why adc are really noisy, 03 september 2024 at 13:34
- looks ESP32 IDF adc getting more stable if `frequency` increased, try to use 20Khz but its not stable, but use 1Mhz its more stable 14 september 2024 at 21:00
- ✔️ LVGL worked but task semaphore block update of LVGL (still investigate), 05 oktober 2024 at 10:13
- ✔️ now LVGL with Button input worked!! 😃, need learn more how to map LVGL key to input button, 05 oktober 2024 at 13:23
- ✔️ sampling data from heater and plot it, 30 oktober 2024 at 2015


<details>
<summary>Several Notes</summary>

## 📔 Note to Myself
this is just my own diy solder reflow made from seferal reference accross internet.
at initial of this project, i want:
  1. use ptc heater instead of iron 
  2. design some case for this project 
  3. use esp32 or wemos
  4. use native toolchain instead arduino 
  6. make good documentation about this project 

## 🐍 Reference
- maker moekoe: https://github.com/makermoekoe/Hotplate-Soldering-Iron  
- casing design: https://cdn.shopify.com/s/files/1/1978/9859/files/DSC_7360.jpg?v=1609830857
 
</details>

<sup> Work In Progress, Made with ♥️ by AH... </sup>

