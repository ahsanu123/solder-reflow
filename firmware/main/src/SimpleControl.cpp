#include "SimpleControl.h"

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
    }
    break;
  case MAINTAIN_HEAT:
    // maintainHeat(temp);
    break;
  case COOLDOWN:
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
