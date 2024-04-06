#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canvas/fonts/fonts.h"
#include "driver/uart.h"
#include "i2c.h"
#include "lcd_hal/esp/esp8266_i2c.h"
#include "src/Drawing.h"
#include "src/Encoder.h"
#include "src/SimpleControl.h"
#include "v2/gui/menu.h"
#include "v2/lcd/ssd1306/lcd_ssd1306.h"

static const char *TAG = "uart_events";

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

#define MENU_SELECT -1
#define MENU_START 0
#define MENU_STOP 1
#define MENU_MONITOR 2
#define MENU_SETPOINT 3
#define MENU_ABOUT 4

// BUG: fix this
float dummyfloat;

static void main_loop(void *pvParameters) {
  /* uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE); */

  DisplaySSD1306_128x32_I2C display(-1);

  const char *mainMenuText[] = {"Start", "Stop", "Monitor", "SetPoint",
                                "About"};
  LcdGfxMenu mainMenu(mainMenuText, sizeof(mainMenuText) / sizeof(char *));
  Encoder encoder;
  PT100 pt100;
  SimpleControl control;
  Drawing drawing(display, pt100);

  encoder.connectedMenu = &mainMenu;

  display.begin();
  display.setFixedFont(ssd1306xled_font6x8);
  display.clear();
  mainMenu.show(display);
  control.sensor = &pt100;

  for (;;) {
    // read input rotary encoder

    encoder.decodeEncoder();

    switch (mainMenu.selection()) {
    case MENU_START:
      control.state = START;
      break;

    case MENU_STOP:
      control.state = COOLDOWN;
      break;

    case MENU_MONITOR:
      if (encoder.decodeSwitch(dummyfloat)) {
        drawing.plotTemp();
      }
      break;

    case MENU_SETPOINT:
      encoder.decodeSwitch(control.setPoint);
      break;

    case MENU_ABOUT:
      break;

    default:
      control.state = IDLE;
      break;
    }

    control.process();
  }

  /* free(dtmp); */
  /* dtmp = NULL; */
  vTaskDelete(NULL);
}

extern "C" void app_main();

void app_main() { xTaskCreate(main_loop, "main_loop", 2048, NULL, 12, NULL); }
