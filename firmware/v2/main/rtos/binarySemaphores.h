/*
 * References:
 * https://github.com/DiegoPaezA/ESP32-freeRTOS/blob/master/Semaforo_Test1/main.c
 *
 * */

#ifndef binarySemaphores
#define binarySemaphores

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "soc/gpio_num.h"

// clang-format off
#define MS_DELAY(TIME) TIME / portTICK_PERIOD_MS
#define INPUT1 gpio_num_t::GPIO_NUM_27
#define INPUT2 gpio_num_t::GPIO_NUM_26
#define INPUT3 gpio_num_t::GPIO_NUM_25
#define INPUT4 gpio_num_t::GPIO_NUM_22

#define GPIO_INPUT_SELECTOR (1<<INPUT1)| \
                            (1<<INPUT2)| \
                            (1<<INPUT3)| \
                            (1<<INPUT4) 

#define OUTPUT1 gpio_num_t::GPIO_NUM_4
#define OUTPUT2 gpio_num_t::GPIO_NUM_21

#define GPIO_OUTPUT_SELECTOR  (1<<OUTPUT1)| \
                              (1<<OUTPUT2)

// clang-format on
SemaphoreHandle_t semaphoreHandle;

static void IRAM_ATTR gpio_ISR(void *arg) {

  BaseType_t base = pdFALSE;
  xSemaphoreGiveFromISR(semaphoreHandle, &base);
  /*ESP_LOGI("INFO", "ISR");*/
  if (base) {
    portYIELD_FROM_ISR(base);
  }
}

void taskThatWaitSemaphoresFromGPIO(void *arg) {
  while (true) {
    if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(200)) == pdPASS) {
      ESP_LOGI("INFO", "GOT ISR NOW DELAYED FOR 1 SECONDS");
      gpio_set_level(OUTPUT1, 1);
      vTaskDelay(MS_DELAY(1000));
      gpio_set_level(OUTPUT1, 0);
    }
  }
}

void demoMainBinarySemaphoresFromISR() {

  semaphoreHandle = xSemaphoreCreateBinary();

  // setup gpio
  gpio_config_t config = {};
  config.mode = GPIO_MODE_INPUT;
  config.intr_type = GPIO_INTR_NEGEDGE;
  config.pull_up_en = GPIO_PULLUP_ENABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.pin_bit_mask = GPIO_INPUT_SELECTOR;

  gpio_config(&config);
  gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
  gpio_isr_handler_add(INPUT1, gpio_ISR, NULL);
  gpio_isr_handler_add(INPUT2, gpio_ISR, NULL);
  gpio_isr_handler_add(INPUT3, gpio_ISR, NULL);
  gpio_isr_handler_add(INPUT4, gpio_ISR, NULL);

  config.mode = GPIO_MODE_OUTPUT;
  config.intr_type = GPIO_INTR_DISABLE;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pin_bit_mask = GPIO_OUTPUT_SELECTOR;
  gpio_config(&config);

  gpio_set_level(OUTPUT1, 0);
  gpio_set_level(OUTPUT2, 0);

  xTaskCreate(taskThatWaitSemaphoresFromGPIO, "taskThatWaitISR", 4096, NULL, 2,
              NULL);
}
#endif
