#include "Encoder.h"
#include <cstdint>

#ifndef INCLUDE_SRC_ENCODER_CPP_
#define INCLUDE_SRC_ENCODER_CPP_

float Encoder::setToTarget(float *target) {
  *target = this->_target;
  return *target;
}

float Encoder::decodeEncoder(uint8_t rawData) {
  /*
   * |New state | Old State|
   * |  3 |  2  |  1 |  0  |
   *
   * */
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
    break;
  case 2:
  case 4:
  case 11:
  case 13:
    this->position -= this->step;
    break;
  case 3:
  case 12:
    this->position += this->step * 2;
    break;
  default:
    this->position -= this->step * 2;
    break;
  }

  this->_oldState = state >> 2;

  return this->position;
};

bool Encoder::decodeSwitch(bool state) {

  /*
   * | OLD || NEW |
   * |  0  ||  0  | no change
   * |  0  ||  1  | toggled
   * |  1  ||  0  | change
   * |  1  ||  1  | no change
   * */

  uint8_t current = this->_oldSwitchState + state;

  if (current == 1) {
    this->_oldSwitchState = state << 1;
    this->switchState = !this->switchState;
    return this->switchState;
  } else if (current == 2) {
    this->_oldSwitchState = state;
    return this->switchState;
  } else {
    return this->switchState;
  }
}
#endif // INCLUDE_SRC_ENCODER_CPP_
