#ifndef TESTER_SSR_WITH_PWM
#define TESTER_SSR_WITH_PWM

#include "../device/ADCDevice.h"
#include "../device/GPIODevice.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/ledc_types.h"

#define HEATER1_PIN GPIO_NUM_4
#define HEATER2_PIN GPIO_NUM_21

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus

Button       *button = new Button();
NewAdcDevice *temp   = new NewAdcDevice();

// FUNCTION
void turnOnHeater(bool state) { /*gpio_set_level(HEATER1_PIN, HIGH);*/ }
void turnOffHeater(bool state) { /*gpio_set_level(HEATER1_PIN, LOW);*/ }

// MAIN
void app_main() {
  ledc_timer_config_t timerConfig = {
    .speed_mode      = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_12_BIT,
    .timer_num       = LEDC_TIMER_0,
    .freq_hz         = 5,
    .clk_cfg         = LEDC_AUTO_CLK
  };

  ledc_timer_config(&timerConfig);

  ledc_channel_config_t channelConfig =
    {.gpio_num   = HEATER1_PIN,
     .speed_mode = LEDC_LOW_SPEED_MODE,
     .channel    = LEDC_CHANNEL_0,
     .duty       = 4096 - (4096 / 3),
     .hpoint     = 0,
     .flags      = {
            .output_invert = false,
     }};
  ledc_channel_config(&channelConfig);

  temp->Init();

  button->Init();
  button->SetOnPressInCallback(turnOnHeater, 1);
  button->SetOnPressInCallback(turnOffHeater, 2);

  int index = 1;
  while (true) {

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4096 - (4096 / index));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    vTaskDelay(pdMS_TO_TICKS(5000));
    index++;
    if (index > 6)
      index = 1;
  }
}
}
#endif

#endif
