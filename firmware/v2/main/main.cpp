#include "control/PID.h"
#include "device/ADCDevice.h"
#include "device/GPIODevice.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/ledc_types.h"

#define HEATER1_PIN GPIO_NUM_4
#define HEATER2_PIN GPIO_NUM_21

#define PID_DT 0.1
#define PID_MAX 4096
#define PID_MIN 0.1
#define PID_KP 17
#define PID_KD 0     // 0.05
#define PID_KI 0.001 // 0.01

#define AVG_COUNT 500

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus

Button       *button = new Button();
NewAdcDevice *temp   = new NewAdcDevice();

// FUNCTION
void turnOnHeater(bool state) { /*gpio_set_level(HEATER1_PIN, HIGH);*/ }
void turnOffHeater(bool state) { /*gpio_set_level(HEATER1_PIN, LOW);*/ }

void init_led_c() {
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
     .duty       = 4096 - (4096 / 20),
     .hpoint     = 0,
     .flags      = {
            .output_invert = false,
     }};
  ledc_channel_config(&channelConfig);
}

void setOutput(double pidValue) {
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, pidValue);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

// MAIN
void app_main() {

  double setPoint = 360;
  init_led_c();
  PID *pid = new PID(PID_DT, PID_MAX, PID_MIN, PID_KP, PID_KD, PID_KI);
  /*pid->setDirection(true);*/

  temp->Init();

  button->Init();
  /*button->SetOnPressInCallback(turnOnHeater, 1);*/
  /*button->SetOnPressInCallback(turnOffHeater, 2);*/

  while (true) {

    temp->Scan();

    double tempValue = (double)raw2temp(temp->GetRawValue(0));
    double output    = pid->calculate(setPoint, setPoint - tempValue, tempValue);
    setOutput(4096.0 - output);

    PIDReturnData_t debugValue = pid->getDebugInfo();
    esp_log_write(
      ESP_LOG_INFO,
      "",
      "%f,%f,%f,%f,%f,%f,%f,%f\n",
      tempValue,
      debugValue.output,
      debugValue.unBoundedOutput,
      debugValue.error,
      debugValue.integral,
      debugValue.derivative,
      debugValue.proportional,
      debugValue.setpoint
    );

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
}
#endif
