
#include "esp_err.h"

#ifndef INCLUDE_DEVICE
#define INCLUDE_DEVICE

class Device {
public:
  virtual esp_err_t Begin() = 0;
};

#endif
