#include "Encoder.h"
#include "gpio.h"
#include <cstdint>

#ifndef INCLUDE_SRC_ENCODER_CPP_
#define INCLUDE_SRC_ENCODER_CPP_

Encoder::Encoder() {

  gpio_config_t enc_ab_sw = {.pin_bit_mask =
                                 GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_15,
                             .mode = GPIO_MODE_INPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&enc_ab_sw);
}

float Encoder::setToTarget(float *target) {
  *target = this->_target;
  return *target;
}

float Encoder::decodeEncoder() {
  /*
   * |New state | Old State|
   * |  3 |  2  |  1 |  0  |
   *
   * */
  uint8_t rawData = (1 << !gpio_get_level(ENC_A)) | !gpio_get_level(ENC_B);
  uint8_t state = this->_oldState & 0x03;

  if (rawData & 0x04)
    state |= 0x04;
  if (rawData & 0x08)
    state |= 0x08;

  switch (state) {
  case 0:
  case 5:
  case 10:
  case 15:
    break;
  case 1:
  case 7:
  case 8:
  case 14:
    this->position += this->step;

    if (this->connectedVariable) {
      *this->connectedVariable += this->step;
    } else {
    }
    this->connectedMenu->up();
    break;
  case 2:
  case 4:
  case 11:
  case 13:
    this->position -= this->step;

    if (this->connectedVariable) {
      *this->connectedVariable -= this->step;
    } else {
    }
    this->connectedMenu->down();
    break;
  case 3:
  case 12:
    this->position += this->step * 2;

    if (this->connectedVariable) {
      *this->connectedVariable += this->step * 2;
    } else {
    }
    this->connectedMenu->up();
    break;
  default:
    this->position -= this->step * 2;

    if (this->connectedVariable) {
      *this->connectedVariable -= this->step * 2;
    } else {
    }
    this->connectedMenu->down();
    break;
  }

  this->_oldState = state >> 2;

  return this->position;
};

bool Encoder::decodeSwitch(float &target) {

  /*
   * | OLD || NEW |
   * |  0  ||  0  | no change
   * |  0  ||  1  | toggled
   * |  1  ||  0  | change
   * |  1  ||  1  | no change
   * */

  bool state = gpio_get_level(ENC_SW);

  uint8_t current = this->_oldSwitchState + state;

  if (current == 1) {
    this->_oldSwitchState = state << 1;
    this->switchState = !this->switchState;

    if (this->switchState) {
      this->connectedVariable = &target;
    } else {
      this->connectedVariable = NULL;
    }

    return this->switchState;
  } else if (current == 2) {
    this->_oldSwitchState = state;
    return this->switchState;
  } else {
    return this->switchState;
  }
}
#endif // INCLUDE_SRC_ENCODER_CPP_
