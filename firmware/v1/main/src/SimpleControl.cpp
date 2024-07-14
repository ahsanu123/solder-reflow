#include "SimpleControl.h"
#include "gpio.h"
#include "hw_timer.h"
#include "lcd_hal/io.h"
#include "portmacro.h"
#include <cstdint>
#include <cstdlib>

#ifndef INCLUDE_SRC_SIMPLECONTROL_CPP_
#define INCLUDE_SRC_SIMPLECONTROL_CPP_

enum SSR_STATE { SSR_STATE_ON = 0, SSR_STATE_OFF, SSR_STATE_CHANGE };

TickType_t lastTime;
int ssrCurrentState = SSR_STATE_OFF; // true on, false off
int ssrLastState = SSR_STATE_OFF;

void turnOnWithDelay(gpio_num_t gpio_num, TickType_t delay) {

  int mappedDelayOn = map(delay, 0, 255, 0, 60);
  int mappedDelayOff = map(delay, 0, 255, 200, 60);

  switch (ssrCurrentState) {
  case SSR_STATE_ON:
    gpio_set_level(SSR_PIN, SSR_ON);
    if (xTaskGetTickCount() > lastTime + mappedDelayOn) {
      lastTime = xTaskGetTickCount();
      ssrCurrentState = SSR_STATE_OFF;
    }
    break;

  case SSR_STATE_OFF:
    gpio_set_level(SSR_PIN, SSR_OFF);
    if (xTaskGetTickCount() > lastTime + mappedDelayOff) {
      lastTime = xTaskGetTickCount();
      ssrCurrentState = SSR_STATE_ON;
    }
    break;
  }
}

int SimpleControl::process(uint8_t state) {

  // calculate diference
  float currentTemp = this->sensor->process();
  float diffTemp = this->_oldTemp[1] - currentTemp;
  this->temp = currentTemp;

  // convert it to percent

  // use turn off with delay to control ssr

  switch (state) {
  case HEATING:
    if (currentTemp < this->setPoint - 3) {
      turnOnWithDelay(SSR_PIN, 20);
    } else if (currentTemp > this->setPoint &&
               currentTemp < this->setPoint + 3) {
      turnOnWithDelay(SSR_PIN, 1000);
    }
    break;

  default:
    break;
  }

  this->_oldTemp[0] = this->_oldTemp[1];
  this->_oldTemp[1] = currentTemp;

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
