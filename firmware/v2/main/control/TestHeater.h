#ifndef TESTER_HEATER
#define TESTER_HEATER

#include "../device/ADCDevice.h"
#include "../device/GPIODevice.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

#define HEATER1_PIN GPIO_NUM_4
#define HEATER2_PIN GPIO_NUM_21

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus

Button       *button = new Button();
NewAdcDevice *temp   = new NewAdcDevice();

// FUNCTION
void turnOnHeater(bool state) {
  /*ESP_LOGI("INFO", "Heater Turned ON");*/
  gpio_set_level(HEATER1_PIN, HIGH);
}
void turnOffHeater(bool state) {
  /*ESP_LOGI("INFO", "Heater Turned OFF");*/
  gpio_set_level(HEATER1_PIN, LOW);
}

// MAIN
void app_main() {
  temp->Init();

  button->Init();
  button->SetOnPressInCallback(turnOnHeater, 1);
  button->SetOnPressInCallback(turnOffHeater, 2);

  initIoAsOutput(HEATER1_PIN);

  while (true) {

    button->Scan();
    temp->Scan();

    esp_log_write(ESP_LOG_INFO, "", "%" PRIu16 "\n", temp->GetRawValue(0));
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
}
#endif

#endif // TESTER_HEATER
