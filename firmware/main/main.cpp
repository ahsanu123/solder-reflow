#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canvas/fonts/fonts.h"
#include "driver/uart.h"
#include "i2c.h"
#include "lcd_hal/esp/esp8266_i2c.h"
#include "src/SimpleControl.h"
#include "v2/gui/menu.h"
#include "v2/lcd/ssd1306/lcd_ssd1306.h"

static const char *TAG = "uart_events";

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
#define MENU_MAIN 0
#define MENU_RUN 1
#define MENU_CONFIG 2
#define MENU_ABOUT 4

static void main_loop(void *pvParameters) {
  /* uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE); */

  DisplaySSD1306_128x32_I2C display(-1);

  const char *menuItems[] = {"Main", "Run", "Config", "About"};
  LcdGfxMenu menu(menuItems, sizeof(menuItems) / sizeof(char *));

  display.setFixedFont(ssd1306xled_font6x8);
  display.begin();

  SimpleControl control;

  for (;;) {
    // read input rotary encoder

    // read input temp

    // run control scheme

    switch (menu.selection()) {
    case MENU_RUN:
      control.state = HEATING;
      break;

    default:
      control.state = IDLE;
      break;
    };

    control.process();
  }

  /* free(dtmp); */
  /* dtmp = NULL; */
  vTaskDelete(NULL);
}

extern "C" void app_main();

void app_main() { xTaskCreate(main_loop, "main_loop", 2048, NULL, 12, NULL); }
