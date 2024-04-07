#include "PT100.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"
#include "portmacro.h"
#include <cstdint>

#define CONTROL_TOLERANCE 3.0
#define CONTROL_MINIMUM_TEMP 160
#define SSR_ON 0
#define SSR_OFF 1
#define SSR_PIN GPIO_NUM_16

#ifdef __cplusplus
extern "C" {
#endif

enum ControlState { IDLE = 0, START, HEATING, MAINTAIN_HEAT, COOLDOWN };

class SimpleControl {

  uint32_t _oldTimer[2];
  float _oldTemp[2];
  float _oldGradient[2];

public:
  float setPoint;
  ControlState state;
  PT100 *sensor;

  int process(uint8_t state);
  void controlMaintainWithPattern(float gradient);
  float getGradient();
  void setNewTimeAndTemp(uint32_t newTime, float newTemp);

  SimpleControl() {
    setPoint = CONTROL_MINIMUM_TEMP;
    state = IDLE;

    gpio_config_t ssr_pin = {.pin_bit_mask = GPIO_Pin_16,
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&ssr_pin);
    gpio_set_level(SSR_PIN, SSR_OFF);
  }

  SimpleControl(float setPoint, ControlState state) : SimpleControl() {
    setPoint = setPoint;
    state = state;
  }
};

#ifdef __cplusplus
}
#endif
