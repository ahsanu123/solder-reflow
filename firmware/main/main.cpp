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

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

enum Menu {
  MENU_SELECT = -1,
  MENU_START1,
  MENU_START2,
  MENU_START3,
  MENU_STOP,
  MENU_MONITOR,
  MENU_SETPOINT,
  MENU_ABOUT
};

// BUG: fix this
float dummyfloat;

static void main_loop(void *pvParameters) {
  /* uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE); */

  DisplaySSD1306_128x32_I2C display(-1);

  hw_timer_enable(true);
  hw_timer_set_reload(false);

  const char *mainMenuText[] = {"Start1",  "Start2",   "Start3", "Stop",
                                "Monitor", "SetPoint", "About"};
  LcdGfxMenu mainMenu(mainMenuText, sizeof(mainMenuText) / sizeof(char *));
  Encoder encoder;
  PT100 pt100;
  SimpleControl control;
  Drawing drawing(display, pt100);

  encoder.connectedMenu = &mainMenu;

  display.setFixedFont(ssd1306xled_font6x8);
  display.begin();
  display.clear();
  mainMenu.show(display);
  control.sensor = &pt100;

  display.clear();
  for (uint8_t y = 0; y < display.height(); y += 8) {
    display.drawLine(0, 0, display.width() - 1, y);
  }
  for (uint8_t x = display.width() - 1; x > 7; x -= 8) {
    display.drawLine(0, 0, x, display.height() - 1);
  }
  lcd_delay(3000);
  bool toggle = false;

  for (;;) {
    // read input rotary encoder

    encoder.decodeEncoder();

    gpio_set_level(SSR_PIN, toggle);

    auto state = mainMenu.selection();
    switch (state) {
    case MENU_START1:
      control.state = START;
      break;

    case MENU_START2:
      break;

    case MENU_START3:
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

    control.process(state);
    vTaskDelay(50);
    toggle = !toggle;
  }

  /* free(dtmp); */
  /* dtmp = NULL; */
  vTaskDelete(NULL);
}

extern "C" void app_main();

void app_main() { xTaskCreate(main_loop, "main_loop", 2048, NULL, 12, NULL); }
