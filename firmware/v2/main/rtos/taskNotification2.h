#ifndef taskNotification2
#define taskNotification2

#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

TaskHandle_t receiverHandler = NULL;
#define MS_DELAY(TIME) TIME / portTICK_PERIOD_MS

void sender(void *param) {
  while (true) {
    xTaskNotify(receiverHandler, (1 << 2), eSetBits);
    vTaskDelay(MS_DELAY(1000));
  }
}

void receiver(void *param) {
  uint32_t state;
  while (true) {

    xTaskNotifyWait(0xffffffff, 0, &state, portMAX_DELAY);
    ESP_LOGI("INFO", "receive notification %" PRIu32 " \n", state);
  }
}

void mainDemoTaskNotificaion2() {
  xTaskCreate(&receiver, "sender", 2048, NULL, 2, &receiverHandler);
  xTaskCreate(&sender, "receiver", 2048, NULL, 2, NULL);
}
#endif
