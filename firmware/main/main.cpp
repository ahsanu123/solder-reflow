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
#include "v2/gui/menu.h"
#include "v2/lcd/ssd1306/lcd_ssd1306.h"

static const char *TAG = "uart_events";

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

static void main_loop(void *pvParameters) {
  /* uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE); */

  DisplaySSD1306_128x32_I2C display(-1);

  const char *menuItems[] = {"Main", "Config", "About"};
  LcdGfxMenu menu(menuItems, sizeof(menuItems) / sizeof(char *));

  display.setFixedFont(ssd1306xled_font6x8);
  display.begin();

  for (;;) {
    // read input rotary encoder

    // read input temp

    // run control scheme
  }

  /* free(dtmp); */
  /* dtmp = NULL; */
  vTaskDelete(NULL);
}

extern "C" void app_main();

void app_main() {
  xTaskCreate(main_loop, "uart_event_task", 2048, NULL, 12, NULL);
}
