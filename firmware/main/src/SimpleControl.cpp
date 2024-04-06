#include "SimpleControl.h"
#include "gpio.h"

#ifndef INCLUDE_SRC_SIMPLECONTROL_CPP_
#define INCLUDE_SRC_SIMPLECONTROL_CPP_

int SimpleControl::process() {

  float temp = this->sensor->process();

  switch (state) {
  case IDLE:
    break;
  case HEATING:
    if (temp > setPoint) {
      // goto maintain
      this->state = MAINTAIN_HEAT;
    } else {
      // heating
      gpio_set_level(SSR_PIN, SSR_ON);
    }
    break;
  case MAINTAIN_HEAT:
    // maintainHeat(temp);
    if (temp > setPoint + CONTROL_TOLERANCE) {
      // turn on heater
      gpio_set_level(SSR_PIN, SSR_OFF);
    } else if (temp < setPoint - CONTROL_TOLERANCE) {
      // turn off heater
      gpio_set_level(SSR_PIN, SSR_ON);
    }
    break;
  case COOLDOWN:
    gpio_set_level(SSR_PIN, SSR_OFF);
    break;
  default:
    break;
  }

  return 0;
}

/* float SimpleControl::heating(float temp) { */
/*   // turn on heater */
/* } */
/**/
/* float SimpleControl::maintainHeat(float temp) { */
/*   if (temp > setPoint + CONTROL_TOLERANCE) { */
/*     // turn on heater */
/*   } else if (temp < setPoint - CONTROL_TOLERANCE) { */
/*     // turn off heater */
/*   } */
/* } */
/**/
/* float SimpleControl::cooldown(float temp) { */
/*   // turn of heater */
/* } */

#endif // INCLUDE_SRC_SIMPLECONTROL_CPP_
