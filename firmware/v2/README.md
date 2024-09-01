# 🏸 Introduction 

SORE - Solder Reflow Firmware Version 2 

## 🧱 How To build 

- make sure you install development environment as described in this [docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
- next add alias `alias get_idf='. $HOME/esp/esp-idf/export.sh'` to your shell profile (.zshrc or other) as described in this [docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#manual-installation)
- next, you can start idf development by run `get_idf`
- to start project run `cmake -S . -B build` then build with `cmake --build build`
- next to upload with builded cmake project you can run `make -C build flash`
- to _serial monitor_ you can use `idf.py -p /dev/ttyUSB0 monitor`. note `/dev/ttyUSB0` is based on your system 
- to open menu config run `idf.py menuconfig`
- 🧴 **another option** to build project is use command `idf.py build`, this command will build project use ninja, also you can _flash_ with `idf.py -p PORT flash` PORT is usb name
- other option use `make` with command `make -C build flash`

## Resource 

- [FreeRtos Coding Standard, Testing and Style Guide](https://www.freertos.org/Documentation/02-Kernel/06-Coding-guidelines/02-FreeRTOS-Coding-Standard-and-Style-Guide#NamingConventions)

## TODO 

- use cxx wrapper [esp-idf-cxx](https://github.com/espressif/esp-idf-cxx)

# Several RTOS Note 

note from Mastering the FreeRTOS Real Time Kernel 

**7.7.2 Considerations When Using a Queue From an ISR**

Queues provide an easy and convenient way of passing data from an interrupt to a task, but it is not efficient
to use a queue if data is arriving at a high frequency.
Many of the demo applications in the FreeRTOS download include a simple UART driver that uses a queue to
pass characters out of the UART's receive ISR. In those demos a queue is used for two reasons: to demonstrate
queues being used from an ISR, and to deliberately load the system in order to test the FreeRTOS port. The
ISRs that use a queue in this manner are definitely not intended to represent an efficient design, and unless the
data is arriving slowly, it is recommended that production code does not copy this technique. More efficient
techniques, that are suitable for production code, include

---

Using Direct Memory Access (DMA) hardware to receive and buffer characters. This method has
practically no software overhead. A direct to task notification[^20] can then be used to unblock the task
that will process the buffer only after a break in transmission has been detected.

Direct to task notifications provide the most efficient method of unblocking a task from an ISR.
Direct to task notifications are covered in Chapter 9, Task Notifications.

---

Copying each received character into a thread safe RAM buffer[^21]. Again, a direct to task notification
can be used to unblock the task that will process the buffer after a complete message has been received,
or after a break in transmission has been detected

---
