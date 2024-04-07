#include "SimpleControl.h"
#include "esp_err.h"
#include "gpio.h"
#include "hw_timer.h"
#include "lcd_hal/io.h"
#include <cstdint>
#include <cstdlib>

#ifndef INCLUDE_SRC_SIMPLECONTROL_CPP_
#define INCLUDE_SRC_SIMPLECONTROL_CPP_

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  if ((in_max - in_min) > (out_max - out_min)) {
    return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) +
           out_min;
  } else {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
}

void turnOnWithDelay(gpio_num_t gpio_num, TickType_t delay) {

  gpio_set_level(gpio_num, SSR_ON);
  vTaskDelay(20);
  gpio_set_level(gpio_num, SSR_OFF);
  vTaskDelay(delay);
}

int SimpleControl::process(uint8_t state) {

  switch (state) {}

  /* float temp = this->sensor->process(); */
  /**/
  /* switch (state) { */
  /* case IDLE: */
  /*   break; */
  /* case HEATING: */
  /*   if (temp > setPoint) { */
  /*     // goto maintain */
  /*     this->state = MAINTAIN_HEAT; */
  /*   } else { */
  /*     // heating */
  /*     gpio_set_level(SSR_PIN, SSR_ON); */
  /*   } */
  /*   break; */
  /* case MAINTAIN_HEAT: */
  /*   // maintainHeat(temp); */
  /*   if (temp > setPoint + CONTROL_TOLERANCE) { */
  /*     // turn on heater */
  /*     gpio_set_level(SSR_PIN, SSR_OFF); */
  /*   } else if (temp < setPoint - CONTROL_TOLERANCE) { */
  /*     // turn off heater */
  /*     gpio_set_level(SSR_PIN, SSR_ON); */
  /**/
  /*     break; */
  /*   case COOLDOWN: */
  /*     gpio_set_level(SSR_PIN, SSR_OFF); */
  /*     break; */
  /*   default: */
  /*     break; */
  /*   } */
  /**/
  /*   return 0; */
  /* } */

  return 0;
}

void SimpleControl::controlMaintainWithPattern(float gradient) {

  float tempNow = this->sensor->process();
  uint32_t timeNow = hw_timer_get_count_data();
  this->setNewTimeAndTemp(timeNow, tempNow);

  float deltaGradient = std::abs(gradient - this->getGradient());
  TickType_t delay = map(deltaGradient, 0, 4, 60, 0);

  turnOnWithDelay(SSR_PIN, delay);
}

float SimpleControl::getGradient() {
  float gradient = (this->_oldTemp[1] - this->_oldTemp[0]) /
                   (this->_oldTimer[1] - this->_oldTimer[0]);
  return gradient;
}

void SimpleControl::setNewTimeAndTemp(uint32_t newTime, float newTemp) {
  this->_oldTemp[0] = this->_oldTemp[1];
  this->_oldTimer[0] = this->_oldTimer[1];

  this->_oldTemp[1] = newTemp;
  this->_oldTimer[1] = newTime;
}

#endif // INCLUDE_SRC_SIMPLECONTROL_CPP_
