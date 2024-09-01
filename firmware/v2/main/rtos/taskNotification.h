/*
 * References:
 * https://github.com/DiegoPaezA/FreeRTOS_Examples/tree/master/TaskNotification_1
 * */

#ifndef taskNotification
#define taskNotification

#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <cstddef>

#define MS_DELAY(TIME) TIME / portTICK_PERIOD_MS
TaskHandle_t receiverHandler = NULL;

void sender(void *param) {
  while (true) {
    xTaskNotifyGive(receiverHandler);
    xTaskNotifyGive(receiverHandler);
    vTaskDelay(MS_DELAY(5000));
  }
}

void receiver(void *param) {
  while (true) {
    int count = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGI("INFO", "receive notification %d \n", count);
  }
}

void mainDemoTaskNotificaion1() {
  xTaskCreate(&receiver, "sender", 2048, NULL, 2, &receiverHandler);
  xTaskCreate(&sender, "receiver", 2048, NULL, 2, NULL);
}
#endif
